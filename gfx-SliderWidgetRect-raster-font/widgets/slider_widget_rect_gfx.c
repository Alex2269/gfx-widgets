/* slider_widget_rect_gfx.c */
#include "slider_widget_rect_gfx.h"
#include "color_utils.h"  // ✅ Додано для ColorBrighten/ColorHover/ColorActive
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

extern int spacing;  // Відступ між символами (з main.c)

/* ================= ГЛОБАЛЬНИЙ СТАН ================= */
typedef struct {
    int x, y, width, height;    // Область слайдера
    float *value;               // Вказівник на значення
    float minValue, maxValue;   // Діапазон
    bool isVertical;            // Орієнтація
    uint32_t baseColor;         // Базовий колір (вже приглушений 0.75f)
    bool isActive;              // Чи захоплений мишею
    bool used;                  // Чи використовується слот
    const char *textTop;        // Підказка зверху
    const char *textRight;      // Підказка праворуч
} RectSliderGfx;

static RectSliderGfx slidersGfx[MAX_RECT_SLIDERS_GFX] = {0};
static int slidersCountGfx = 0;
static int activeSliderIndexGfx = -1;

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

/* Перевірка чи точка всередині прямокутника */
static bool PointInRectGfx(int px, int py, int rx, int ry, int rw, int rh) {
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

/* Перевірка чи миша в зоні захоплення ручки */
static bool IsMouseNearRectKnobGfx(RectSliderGfx *s) {
    if (!s->used || !s->value) return false;
    
    float range = s->maxValue - s->minValue;
    if (range == 0) range = 1;
    float norm = (*s->value - s->minValue) / range;
    if (norm < 0) norm = 0; if (norm > 1) norm = 1;
    
    int knobX, knobY;
    if (s->isVertical) {
        knobX = s->x + s->width / 2;
        knobY = s->y + (int)((1.0f - norm) * s->height);
    } else {
        knobX = s->x + (int)(norm * s->width);
        knobY = s->y + s->height / 2;
    }
    
    int pad = CAPTURE_PAD_GFX;
    return PointInRectGfx(gfx_get_mouse_x(), gfx_get_mouse_y(),
                          knobX - KNOB_WIDTH_GFX/2 - pad,
                          knobY - KNOB_HEIGHT_GFX/2 - pad,
                          KNOB_WIDTH_GFX + 2*pad,
                          KNOB_HEIGHT_GFX + 2*pad);
}

/* Оновлення значення з позиції миші */
static void UpdateValueFromMouseGfx(RectSliderGfx *s) {
    if (!s->value) return;
    
    float norm;
    if (s->isVertical) {
        norm = 1.0f - (float)(gfx_get_mouse_y() - s->y) / s->height;
    } else {
        norm = (float)(gfx_get_mouse_x() - s->x) / s->width;
    }
    if (norm < 0) norm = 0; if (norm > 1) norm = 1;
    
    *s->value = s->minValue + norm * (s->maxValue - s->minValue);
}

/* Малювання прямокутної ручки з ефектом */
static void DrawRectKnobGfx(int cx, int cy, uint32_t color) {
    int x = cx - KNOB_WIDTH_GFX/2;
    int y = cy - KNOB_HEIGHT_GFX/2;
    
    // Тінь
    DrawRectangleFast(x+1, y+1, KNOB_WIDTH_GFX, KNOB_HEIGHT_GFX, 0x404040);
    // Основна ручка
    DrawRectangleFast(x, y, KNOB_WIDTH_GFX, KNOB_HEIGHT_GFX, color);
    // Блик
    DrawRectangleFast(x+1, y+1, KNOB_WIDTH_GFX-2, 2, 0xA0A0A0);
    // Контур
    DrawRectangleLinesFast(x, y, KNOB_WIDTH_GFX, KNOB_HEIGHT_GFX, GetContrastColor(color));
}

/* Вимірювання ширини тексту */
static int MeasureTextGfx(RasterFont font, const char *text, int sp) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + sp) - sp;
}

/* ================= ПУБЛІЧНІ ФУНКЦІЇ ================= */

void RegisterRectKnobSliderGfx(int idx, int x, int y, int w, int h,
                               float *val, float minV, float maxV,
                               bool vert, uint32_t col,
                               const char *tTop, const char *tRight) {
    if (idx < 0 || idx >= MAX_RECT_SLIDERS_GFX) return;
    
    if (!slidersGfx[idx].used) {
        slidersGfx[idx].used = true;
        if (idx >= slidersCountGfx) slidersCountGfx = idx + 1;
    }
    
    slidersGfx[idx].x = x;
    slidersGfx[idx].y = y;
    slidersGfx[idx].width = w;
    slidersGfx[idx].height = h;
    slidersGfx[idx].value = val;
    slidersGfx[idx].minValue = minV;
    slidersGfx[idx].maxValue = maxV;
    slidersGfx[idx].isVertical = vert;
    
    // ✅ Приглушуємо базовий колір на 25% для запасу яскравості
    slidersGfx[idx].baseColor = ColorBrighten(col, 0.75f);
    
    slidersGfx[idx].textTop = tTop;
    slidersGfx[idx].textRight = tRight;
    slidersGfx[idx].isActive = false;
}

void UpdateRectKnobSlidersAndDrawGfx(RasterFont font, int sp) {
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    
    /* --- Обробка миші --- */
    // Початок перетягування
    if (gfx_is_mouse_pressed(GFX_MOUSE_LEFT) && activeSliderIndexGfx == -1) {
        for (int i = slidersCountGfx - 1; i >= 0; i--) {
            if (slidersGfx[i].used && IsMouseNearRectKnobGfx(&slidersGfx[i])) {
                for (int k = 0; k < slidersCountGfx; k++) slidersGfx[k].isActive = false;
                activeSliderIndexGfx = i;
                slidersGfx[i].isActive = true;
                break;
            }
        }
    }
    // Завершення перетягування
    if (gfx_is_mouse_released(GFX_MOUSE_LEFT)) {
        for (int k = 0; k < slidersCountGfx; k++) slidersGfx[k].isActive = false;
        activeSliderIndexGfx = -1;
    }
    // Оновлення під час перетягування
    if (activeSliderIndexGfx != -1 && gfx_is_mouse_down(GFX_MOUSE_LEFT)) {
        UpdateValueFromMouseGfx(&slidersGfx[activeSliderIndexGfx]);
    }
    
    /* --- Малювання треків та звичайних ручок --- */
    for (int i = 0; i < slidersCountGfx; i++) {
        RectSliderGfx *s = &slidersGfx[i];
        if (!s->used || !s->value) continue;
        
        float range = s->maxValue - s->minValue;
        if (range == 0) range = 1;
        float norm = (*s->value - s->minValue) / range;
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;
        
        // ✅ Фон треку: нейтральний світло-сірий для контрасту
        DrawRectangleFast(s->x, s->y, s->width, s->height, 0xE8E8E8);
        DrawRectangleLinesFast(s->x, s->y, s->width, s->height, GetContrastColor(s->baseColor));
        
        // ✅ Колір ручки через ColorBrighten (надійна зміна яскравості)
        uint32_t knobColor = s->baseColor;
        if (!s->isActive && IsMouseNearRectKnobGfx(s)) {
            // Hover-ефект: +15% яскравості
            knobColor = ColorBrighten(s->baseColor, 1.15f);
        }
        
        // Позиція ручки
        int knobX, knobY;
        if (s->isVertical) {
            knobX = s->x + s->width / 2;
            knobY = s->y + (int)((1.0f - norm) * s->height);
        } else {
            knobX = s->x + (int)(norm * s->width);
            knobY = s->y + s->height / 2;
        }
        
        DrawRectKnobGfx(knobX, knobY, knobColor);
    }
    
    /* --- Активна ручка поверх усіх --- */
    if (activeSliderIndexGfx != -1) {
        RectSliderGfx *s = &slidersGfx[activeSliderIndexGfx];
        float range = s->maxValue - s->minValue;
        if (range == 0) range = 1;
        float norm = (*s->value - s->minValue) / range;
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;
        
        // ✅ Активна ручка: +30% яскравості через ColorBrighten
        uint32_t activeColor = ColorBrighten(s->baseColor, 1.3f);
        
        int knobX, knobY;
        if (s->isVertical) {
            knobX = s->x + s->width / 2;
            knobY = s->y + (int)((1.0f - norm) * s->height);
        } else {
            knobX = s->x + (int)(norm * s->width);
            knobY = s->y + s->height / 2;
        }
        DrawRectKnobGfx(knobX, knobY, activeColor);
    }
    
    /* --- Підказки та значення (тільки для одного активного/наведеного) --- */
    for (int i = slidersCountGfx - 1; i >= 0; i--) {
        RectSliderGfx *s = &slidersGfx[i];
        if (!s->used) continue;
        
        if (s->isActive || IsMouseNearRectKnobGfx(s)) {
            int pad = 6;
            
            // textTop (зверху)
            if (s->textTop && s->textTop[0]) {
                char tmp[256];
                strncpy(tmp, s->textTop, sizeof(tmp)-1);
                tmp[sizeof(tmp)-1] = '\0';
                
                const char *lines[10];
                int lc = 0;
                char *line = strtok(tmp, "\n");
                while (line && lc < 10) { lines[lc++] = line; line = strtok(NULL, "\n"); }
                
                int maxW = 0;
                for (int li = 0; li < lc; li++) {
                    int tw = MeasureTextGfx(font, lines[li], sp);
                    if (tw > maxW) maxW = tw;
                }
                
                int lh = font.glyph_height;
                int totalH = lc * lh + (lc-1)*2 + 2*pad;
                int tx = s->x + s->width/2 - maxW/2;
                int ty = s->y - totalH - 8;
                
                uint32_t bg = GetContrastInvertColor(s->baseColor);
                DrawRectangleFast(tx - pad, ty - pad, maxW + 2*pad, totalH + 2*pad, bg);
                DrawRectangleLinesFast(tx - pad, ty - pad, maxW + 2*pad, totalH + 2*pad, GetContrastColor(bg));
                
                for (int li = 0; li < lc; li++) {
                    DrawTextScaled(font, tx, ty + li*(lh+2), lines[li], sp, 1, GetContrastColor(bg));
                }
            }
            
            // textRight (праворуч)
            if (s->textRight && s->textRight[0]) {
                char tmp[256];
                strncpy(tmp, s->textRight, sizeof(tmp)-1);
                tmp[sizeof(tmp)-1] = '\0';
                
                const char *lines[10];
                int lc = 0;
                char *line = strtok(tmp, "\n");
                while (line && lc < 10) { lines[lc++] = line; line = strtok(NULL, "\n"); }
                
                int maxW = 0;
                for (int li = 0; li < lc; li++) {
                    int tw = MeasureTextGfx(font, lines[li], sp);
                    if (tw > maxW) maxW = tw;
                }
                
                int lh = font.glyph_height;
                int totalH = lc * lh + (lc-1)*2 + 2*pad;
                int tx = s->x + s->width + 10;
                int ty = s->y + s->height/2 - totalH/2;
                
                uint32_t bg = GetContrastInvertColor(s->baseColor);
                DrawRectangleFast(tx - pad, ty - pad, maxW + 2*pad, totalH + 2*pad, bg);
                DrawRectangleLinesFast(tx - pad, ty - pad, maxW + 2*pad, totalH + 2*pad, GetContrastColor(bg));
                
                for (int li = 0; li < lc; li++) {
                    DrawTextScaled(font, tx, ty + li*(lh+2), lines[li], sp, 1, GetContrastColor(bg));
                }
            }
            
            // Числове значення
            char valStr[16];
            snprintf(valStr, sizeof(valStr), "%.2f", *s->value);
            int tw = MeasureTextGfx(font, valStr, sp);
            int tx = s->x + s->width + 12;
            int ty = s->y + s->height/2 + 20;
            
            uint32_t bg = GetContrastInvertColor(s->baseColor);
            DrawRectangleFast(tx - pad, ty - pad, tw + 2*pad, font.glyph_height + 2*pad, bg);
            DrawRectangleLinesFast(tx - pad, ty - pad, tw + 2*pad, font.glyph_height + 2*pad, GetContrastColor(bg));
            DrawTextScaled(font, tx, ty, valStr, sp, 1, GetContrastColor(bg));
            
            break;  // Показуємо підказки тільки для одного слайдера
        }
    }
}
