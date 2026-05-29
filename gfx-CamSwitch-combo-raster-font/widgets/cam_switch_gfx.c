/* widgets/cam_switch_gfx.c */
#include "cam_switch_gfx.h"
#include "color_utils.h"
#include "graphics.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define PI 3.14159265358979323846f
#define CHANNEL_COUNT 6

static float camAngles[CHANNEL_COUNT] = { -135.0f, -135.0f, -135.0f, -135.0f, -135.0f, -135.0f };
static bool isDragging[CHANNEL_COUNT] = { false, false, false, false, false, false };
static int activeDrag = -1;

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */
static int dist_sq(int x1, int y1, int x2, int y2) {
    int dx = x1-x2, dy = y1-y2; return dx*dx + dy*dy;
}
static bool mouse_over_circle(int cx, int cy, int r) {
    return dist_sq(cx, cy, gfx_get_mouse_x(), gfx_get_mouse_y()) <= r*r;
}
static void GfxDrawCircle(int cx, int cy, int r, uint32_t color) {
    for(int y=-r; y<=r; y++)
        for(int x=-r; x<=r; x++)
            if(x*x + y*y <= r*r) DrawPixel(cx+x, cy+y, color);
}
static void FormatValue(char *buf, int size, float val, float mn, float mx) {
    float maxAbs = (fabsf(mn) > fabsf(mx)) ? fabsf(mn) : fabsf(mx);
    int prec = (maxAbs < 10.0f) ? 1 : 0;
    char fmt[8]; snprintf(fmt, sizeof(fmt), "%%.%df", prec);
    snprintf(buf, size, fmt, val);
}

static float NormalizeVal(float v, float mn, float mx) {
    return (mn < mx) ? (v - mn) / (mx - mn) : 1.0f - (v - mx) / (mn - mx);
}
static float DenormalizeVal(float n, float mn, float mx) {
    return (mn < mx) ? mn + n * (mx - mn) : mx + (1.0f - n) * (mn - mx);
}

/* Логарифмічна шкала */
static const float LOG_SCALE[] = {2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000};
#define LOG_COUNT 11
static int FindLogIdx(float v) {
    int idx=0; float best=1e9f;
    for(int k=0;k<LOG_COUNT;k++){ float d=fabsf(v-LOG_SCALE[k]); if(d<best){best=d;idx=k;} }
    return idx;
}
static float AngleFromLogIdx(int idx) {
    return ((float)idx / (LOG_COUNT - 1)) * 270.0f - 135.0f;
}

/* ================= МАЛЮВАННЯ РУЧКИ ================= */
static void draw_knob(RasterFont f_val, int x, int y, float r, float angle, float value,
                      float mn, float mx, uint32_t colorTxt) {
    // Фон ручки
    GfxDrawCircle(x, y, (int)r, LIGHTGRAY);
    GfxDrawCircle(x, y, (int)r - 4, DARKGRAY);

    // Кольорові сектори (товсті та охайні)
    // Кольорові сектори (швидко, без пропусків, товщина 5px)
    struct Zone { float s, e; uint32_t c; } zones[] = {
        {0.00f, 0.33f, GREEN}, {0.33f, 0.66f, YELLOW}, {0.66f, 1.00f, RED}
    };
    for(int i = 0; i < 3; i++) {
        float a1 = -135.0f + zones[i].s * 270.0f;
        float a2 = -135.0f + zones[i].e * 270.0f;

        // Встановлюємо колір один раз на весь сектор
        gfx_color(zones[i].c);  // zones[i].c вже є uint32_t кольором!

        // 5 паралельних дуг = товста смуга без дірок
        for(int t = -2; t <= 2; t++) {
            float rad = (a1 - 90.0f) * (PI/180.0f);
            int x1 = x + (int)(cosf(rad) * (r - 10 + t));
            int y1 = y + (int)(sinf(rad) * (r - 10 + t));

            for(float a = a1 + 1.5f; a <= a2; a += 0.5f) {
                rad = (a - 90.0f) * (PI/180.0f);
                int x2 = x + (int)(cosf(rad) * (r - 10 + t));
                int y2 = y + (int)(sinf(rad) * (r - 10 + t));
                gfx_line(x1, y1, x2, y2);
                x1 = x2; y1 = y2;
            }
        }
    }

    // ✅ Стрілка (тонша + кружечок на кінці)
    float rad = (angle - 90.0f) * (PI/180.0f);
    Vector2 center = {(float)x, (float)y};
    Vector2 tip = {x + (int)(cosf(rad) * (r - 6)), y + (int)(sinf(rad) * (r - 6))};
    DrawLineEx(center, tip, 2, colorTxt);           // Тонка основа
    GfxDrawCircle((int)tip.x, (int)tip.y, 4, colorTxt); // Наконечник-коло

    // ✅ Текст значення (КОЛІР СТРІЛКИ) + контрастний фон
    char buf[32]; FormatValue(buf, sizeof(buf), value, mn, mx);
    int chars = utf8_strlen(buf);
    float tw = chars * (f_val.glyph_width + 2) - 2;
    float th = f_val.glyph_height;
    int px = 4, py = 2;
    int rx = x - (int)(tw/2) - px;
    int ry = y + (int)r + 6;
    
    // Фон автоматично контрастний до кольору стрілки/тексту
    uint32_t bg = GetContrastInvertColor(colorTxt);
    DrawRectangleFast(rx, ry, (int)tw + 2*px, (int)th + 2*py, bg);
    DrawRectangleLines(rx, ry, (int)tw + 2*px, (int)th + 2*py, colorTxt); // Рамка кольору стрілки
    DrawTextScaled(f_val, rx+px, ry+py, buf, 2, 1, colorTxt); // ✅ Текст кольору стрілки!
}

/* ================= ОБРОБКА МИШІ ================= */
static float handle_knob(int x, int y, float r, float val, bool *drag, float *ang,
                         bool active, int ch, float mn, float mx, bool log) {
    if(!active) return val;
    bool over = mouse_over_circle(x, y, r);

    // ✅ ВИПРАВЛЕНО: Кут від миші з правильною конвертацією
    float a = atan2f(gfx_get_mouse_y()-y, gfx_get_mouse_x()-x) * (180.0f/PI);
    a += 90.0f;  // ✅ ПЛЮС замість мінуса: конвертуємо екранний кут → UI кут

    // Нормалізуємо до [-180, 180] для коректної обробки меж
    if(a > 180.0f) a -= 360.0f;
    if(a <= -180.0f) a += 360.0f;

    // Обмежуємо діапазоном дуги [-135, +135]
    if(a < -135.0f) a = -135.0f;
    if(a >  135.0f) a =  135.0f;

    if(gfx_is_mouse_pressed(1) && over && activeDrag == -1) {
        *drag = true; activeDrag = ch;
    }
    if(gfx_is_mouse_released(1) && *drag) {
        *drag = false; activeDrag = -1;
    }

    if(*drag) {
        *ang = a;
        float norm = (*ang + 135.0f) / 270.0f;
        if(log) {
            int idx = (int)roundf(norm * (LOG_COUNT-1));
            if(idx<0) idx=0; if(idx>=LOG_COUNT) idx=LOG_COUNT-1;
            val = LOG_SCALE[idx];
        } else {
            val = DenormalizeVal(norm, mn, mx);
            if(mn<mx){ if(val<mn)val=mn; if(val>mx)val=mx; }
            else{ if(val>mn)val=mn; if(val<mx)val=mx; }
        }
    }
    return val;
}

/* ================= ГОЛОВНИЙ ВІДЖЕТ ================= */
int Gui_CamSwitch_GFX(int ch, RasterFont f_knob, RasterFont f_val,
                      int x, int y, const char *tipTop, const char *txtRight,
                      float r, float *val, float mn, float mx,
                      bool active, uint32_t cTxt, bool log) {
    if(ch<0 || ch>=CHANNEL_COUNT) return 0;

    // Фон віджета
    int bw = (int)(r/10*37), bh = (int)(r/10*31);
    DrawRectangleFast(x-bw/2, y-bh/2-5, bw, bh, GetContrastColor(cTxt));

    *val = handle_knob(x, y, r, *val, &isDragging[ch], &camAngles[ch], active, ch, mn, mx, log);
    bool changed = false;
    int ticks = log ? LOG_COUNT : 11;

    // Фіксація після drag / scroll
    if(!isDragging[ch]) {
        if(log) {
            int idx = FindLogIdx(*val);
            *val = LOG_SCALE[idx];
            camAngles[ch] = AngleFromLogIdx(idx);
        } else {
            float norm = NormalizeVal(*val, mn, mx);
            float rn = roundf(norm*(ticks-1))/(ticks-1);
            *val = DenormalizeVal(rn, mn, mx);
            camAngles[ch] = rn * 270.0f - 135.0f;
        }
    }

    draw_knob(f_val, x, y, r, camAngles[ch], *val, mn, mx, cTxt);

    // Колесо миші
    bool over = mouse_over_circle(x, y, r);
    if(over) {
        int wh = gfx_get_mouse_wheel();
        if(wh != 0) {
            if(log) {
                int idx = FindLogIdx(*val) + wh;
                if(idx<0) idx=0; if(idx>=LOG_COUNT) idx=LOG_COUNT-1;
                *val = LOG_SCALE[idx];
                camAngles[ch] = AngleFromLogIdx(idx);
            } else {
                float step = fabsf(mx-mn)/(ticks-1);
                *val += wh * step;
                if(mn<mx){ if(*val<mn)*val=mn; if(*val>mx)*val=mx; }
                else{ if(*val>mn)*val=mn; if(*val<mx)*val=mx; }
                float norm = NormalizeVal(*val, mn, mx);
                camAngles[ch] = norm * 270.0f - 135.0f;
            }
            changed = true;
        }
    }

    // Ділення шкали
    for(int i=0; i<ticks; i++) {
        float tv = log ? LOG_SCALE[i] : mn + i*(mx-mn)/(ticks-1);
        float aDeg = -135.0f + i*(270.0f/(ticks-1));
        float aRad = (aDeg - 90.0f)*(PI/180.0f);
        Vector2 p1 = {x + cosf(aRad)*(r+8), y + sinf(aRad)*(r+8)};
        Vector2 p2 = {x + cosf(aRad)*r, y + sinf(aRad)*r};
        DrawLineEx(p1, p2, 2, cTxt);

        char buf[16]; FormatValue(buf, sizeof(buf), tv, mn, mx);
        int cc = utf8_strlen(buf);
        float tr = r + 10 + (f_knob.glyph_width * cc)/2 + 4;
        int tx = x + cosf(aRad)*tr - (cc*(f_knob.glyph_width+2)-2)/2;
        int ty = y + sinf(aRad)*tr - f_knob.glyph_height/2;
        DrawTextScaled(f_knob, tx, ty, buf, 2, 1, cTxt);
    }

    // Tooltip
    if(over && tipTop && tipTop[0]) {
        int pad=6;
        char tmp[128]; strncpy(tmp, tipTop, sizeof(tmp)-1); tmp[sizeof(tmp)-1]='\0';
        char *line = strtok(tmp, "\n");
        int lc=0; const char *lines[5];
        while(line && lc<5){ lines[lc++]=line; line=strtok(NULL,"\n"); }
        
        float maxW=0;
        for(int i=0;i<lc;i++){ int cc=utf8_strlen(lines[i]); maxW = fmaxf(maxW, cc*(f_val.glyph_width+2)-2); }
        float lh = f_val.glyph_height;
        float th = lc*lh + pad*2;
        int rx = x - maxW/2 - pad;
        int ry = y - (int)r - (int)th - pad;
        DrawRectangleFast(rx, ry, (int)maxW+2*pad, (int)th, cTxt);
        for(int i=0;i<lc;i++)
            DrawTextScaled(f_val, rx+pad, ry+pad+i*(lh+2), lines[i], 2, 1, GetContrastColor(cTxt));
    }

    if(txtRight && txtRight[0])
        DrawTextScaled(f_val, x+(int)r+10, y-10, txtRight, 1, 1, cTxt);

    return changed ? 1 : 0;
}
