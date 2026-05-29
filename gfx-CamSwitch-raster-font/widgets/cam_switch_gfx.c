/* cam_switch_gfx.c */
#include "cam_switch_gfx.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846f
#define MAX_CAM_SWITCHES_GFX 10

/* ================= ГЛОБАЛЬНИЙ СТАН ================= */
typedef struct {
    float angle;
    bool isDragging;
} CamSwitchStateGfx;

static CamSwitchStateGfx switchesState[MAX_CAM_SWITCHES_GFX] = {0};
static int activeDragGfx = -1;

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

/* Малювання заповненого кола (scanline, швидко для gfx) */
static void DrawFilledCircleGfx(int cx, int cy, int r, uint32_t color) {
    if (r <= 0) return;
    gfx_set_color_uint32(color);
    for (int y = -r; y <= r; y++) {
        int dx = (int)sqrtf((float)r*r - (float)y*y);
        gfx_line(cx - dx, cy + y, cx + dx, cy + y);
    }
}

/* Перевірка попадання миші в коло */
static bool MouseOverCircleGfx(int cx, int cy, int r) {
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    int dx = mx - cx, dy = my - cy;
    return (dx*dx + dy*dy) <= (r*r);
}

/* Форматування значення (автоматична точність) */
static void FormatCamValueGfx(char *buf, int size, float val, float mn, float mx) {
    float maxAbs = (fabsf(mn) > fabsf(mx)) ? fabsf(mn) : fabsf(mx);
    int prec = (maxAbs < 10.0f) ? 1 : 0;
    char fmt[8]; 
    snprintf(fmt, sizeof(fmt), "%%.%df", prec);
    snprintf(buf, size, fmt, val);
}

/* Нормалізація значення [0..1] з підтримкою інверсії */
static float NormalizeValGfx(float v, float mn, float mx) {
    return (mn < mx) ? (v - mn) / (mx - mn) : 1.0f - (v - mx) / (mn - mx);
}

/* Денормалізація [0..1] -> значення */
static float DenormalizeValGfx(float n, float mn, float mx) {
    return (mn < mx) ? mn + n * (mx - mn) : mx + (1.0f - n) * (mn - mx);
}

/* Вимірювання ширини тексту */
static int MeasureTextGfx(RasterFont font, const char *text, int sp) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + sp) - sp;
}

/* Малювання градієнтного кільця (імітація через кілька кіл) */
static void DrawGradientRingGfx(int cx, int cy, float innerR, float outerR, 
                                 uint32_t innerColor, uint32_t outerColor, int segments) {
    float step = (outerR - innerR) / segments;
    for (int i = 0; i < segments; i++) {
        float r1 = innerR + i * step;
        float r2 = r1 + step;
        float t = (float)i / (segments - 1);
        
        // Інтерполяція кольору
        uint8_t r1c = (innerColor >> 16) & 0xFF, g1c = (innerColor >> 8) & 0xFF, b1c = innerColor & 0xFF;
        uint8_t r2c = (outerColor >> 16) & 0xFF, g2c = (outerColor >> 8) & 0xFF, b2c = outerColor & 0xFF;
        uint8_t rc = (uint8_t)((1-t)*r1c + t*r2c);
        uint8_t gc = (uint8_t)((1-t)*g1c + t*g2c);
        uint8_t bc = (uint8_t)((1-t)*b1c + t*b2c);
        uint32_t col = (rc << 16) | (gc << 8) | bc;
        
        // Малюємо кільце через два кола (спрощено)
        DrawFilledCircleGfx(cx, cy, (int)r2, col);
        DrawFilledCircleGfx(cx, cy, (int)r1, 0xF0F0F0);  /* "Вирізаємо" внутрішню частину */
    }
}

/* ================= МАЛЮВАННЯ РУЧКИ З СЕКТОРАМИ ================= */
static void DrawCamSwitchKnobGfx(int cx, int cy, float radius, float angle,
                                  float value, float minValue, float maxValue,
                                  uint32_t textColor, RasterFont font) {
    int r = (int)radius;
    if (r < 10) return;

    /* 1. Фон ручки */
    DrawFilledCircleGfx(cx, cy, r, 0xD3D3D3);  /* LIGHTGRAY */
    DrawFilledCircleGfx(cx, cy, r - 4, 0x404040);  /* DARKGRAY */

    /* 2. Градієнтне кільце (декор) */
    DrawGradientRingGfx(cx, cy, r - 12, r - 5, 0xC8C8C8, 0x969696, 20);

    /* 3. Кольорові сектори (GREEN/YELLOW/RED) — товсті дуги */
    struct Zone { float startNorm, endNorm; uint32_t color; } zones[] = {
        {0.00f, 0.33f, 0x00FF00},  /* GREEN */
        {0.33f, 0.66f, 0xFFFF00},  /* YELLOW */
        {0.66f, 1.00f, 0xFF0000}   /* RED */
    };

    for (int i = 0; i < 3; i++) {
        float a1 = -135.0f + zones[i].startNorm * 270.0f;
        float a2 = -135.0f + zones[i].endNorm * 270.0f;
        gfx_set_color_uint32(zones[i].color);

        float trackRadius = r * 0.85f;
        for (int t = -2; t <= 2; t++) {
            float rad = (a1 - 90.0f) * (PI/180.0f);
            int x1 = cx + (int)(cosf(rad) * (trackRadius + t));
            int y1 = cy + (int)(sinf(rad) * (trackRadius + t));

            for (float a = a1 + 1.5f; a <= a2; a += 0.5f) {
                rad = (a - 90.0f) * (PI/180.0f);
                int x2 = cx + (int)(cosf(rad) * (trackRadius + t));
                int y2 = cy + (int)(sinf(rad) * (trackRadius + t));
                gfx_line(x1, y1, x2, y2);
                x1 = x2; y1 = y2;
            }
        }
    }

    /* 4. Стрілка-індикатор */
    float rad = (angle - 90.0f) * (PI/180.0f);
    int tipX = cx + (int)(cosf(rad) * (r - 6));
    int tipY = cy + (int)(sinf(rad) * (r - 6));
    
    gfx_set_color_uint32(textColor);
    gfx_line(cx, cy, tipX, tipY);  /* Основа */
    DrawFilledCircleGfx(tipX, tipY, 4, textColor);  /* Наконечник */

    /* 5. Текст значення з контрастним фоном */
    char buf[32];
    FormatCamValueGfx(buf, sizeof(buf), value, minValue, maxValue);
    
    int chars = utf8_strlen(buf);
    int tw = chars * (font.glyph_width + 2) - 2;
    int th = font.glyph_height;
    int px = 4, py = 2;
    int rx = cx - tw/2 - px;
    int ry = cy + r + 8;

    uint32_t bgColor = GetContrastInvertColor(textColor);
    DrawRectangleFast(rx, ry, tw + 2*px, th + 2*py, bgColor);
    DrawRectangleLinesFast(rx, ry, tw + 2*px, th + 2*py, textColor);
    DrawTextScaled(font, rx + px, ry + py, buf, 2, 1, textColor);
}

/* ================= ОБРОБКА МИШІ ================= */
static float HandleCamSwitchMouseGfx(int cx, int cy, float radius, float value,
                                      bool *isDragging, float *angle,
                                      int id, float minValue, float maxValue) {
    bool mouseOver = MouseOverCircleGfx(cx, cy, (int)radius);

    /* Обчислення кута від миші */
    float a = atan2f(gfx_get_mouse_y() - cy, gfx_get_mouse_x() - cx) * (180.0f / PI);
    a += 90.0f;  /* Конвертація екранного кута → UI кут */

    /* Нормалізація до [-180, 180] */
    if (a > 180.0f) a -= 360.0f;
    if (a <= -180.0f) a += 360.0f;

    /* Обмеження діапазоном дуги [-135, +135] */
    if (a < -135.0f) a = -135.0f;
    if (a > 135.0f) a = 135.0f;

    /* Початок перетягування */
    if (gfx_is_mouse_pressed(GFX_MOUSE_LEFT) && mouseOver && activeDragGfx == -1) {
        *isDragging = true;
        activeDragGfx = id;
    }

    /* Завершення перетягування */
    if (gfx_is_mouse_released(GFX_MOUSE_LEFT) && *isDragging) {
        *isDragging = false;
        activeDragGfx = -1;
    }

    /* Оновлення під час перетягування */
    if (*isDragging) {
        *angle = a;
        float norm = (*angle + 135.0f) / 270.0f;
        value = DenormalizeValGfx(norm, minValue, maxValue);

        /* Clamp з підтримкою інверсії */
        float mn = fminf(minValue, maxValue);
        float mx = fmaxf(minValue, maxValue);
        if (value < mn) value = mn;
        if (value > mx) value = mx;
    }

    return value;
}

/* ================= ГОЛОВНИЙ ВІДЖЕТ ================= */
int Gui_CamSwitch_GFX(int id, int cx, int cy, float radius,
                      float *value, float minValue, float maxValue,
                      uint32_t textColor, const char *textTop, 
                      const char *textRight, RasterFont font, int spacing) {
    if (id < 0 || id >= MAX_CAM_SWITCHES_GFX || !value) return 0;

    CamSwitchStateGfx *state = &switchesState[id];
    bool changed = false;

    /* Фон віджета */
    int bgWidth = (int)(radius / 10 * 37);
    int bgHeight = (int)(radius / 10 * 31);
    DrawRectangleFast(cx - bgWidth/2, cy - bgHeight/2 - 5, bgWidth, bgHeight, 0xE0E0E0);

    /* 1. Обробка миші (drag) */
    *value = HandleCamSwitchMouseGfx(cx, cy, radius, *value, &state->isDragging,
                                      &state->angle, id, minValue, maxValue);

    /* 2. Синхронізація кута зі значенням (без джейтера) */
    if (!state->isDragging) {
        float norm = NormalizeValGfx(*value, minValue, maxValue);
        state->angle = norm * 270.0f - 135.0f;
    }

    /* 3. Колесо миші */
    if (MouseOverCircleGfx(cx, cy, (int)radius) && !state->isDragging) {
        int wheel = gfx_get_mouse_wheel();
        if (wheel != 0) {
            float range = fabsf(maxValue - minValue);
            float step = fmaxf(0.1f, range / 100.0f);  /* 1% від діапазону */
            
            /* ✅ Скрол вгору = збільшення */
            *value += wheel * step;

            /* Clamp з підтримкою інверсії */
            float mn = fminf(minValue, maxValue);
            float mx = fmaxf(minValue, maxValue);
            if (*value < mn) *value = mn;
            if (*value > mx) *value = mx;

            state->angle = NormalizeValGfx(*value, minValue, maxValue) * 270.0f - 135.0f;
            changed = true;
        }
    }

    /* 4. Малювання ручки з секторами */
    DrawCamSwitchKnobGfx(cx, cy, radius, state->angle, *value,
                         minValue, maxValue, textColor, font);

    /* 5. Малювання поділок шкали */
    int ticks = 11;
    for (int i = 0; i < ticks; i++) {
        float tickValue = minValue + i * (maxValue - minValue) / (ticks - 1);
        float tickAngle = -135.0f + i * (270.0f / (ticks - 1));
        float tickRad = (tickAngle - 90.0f) * (PI / 180.0f);

        /* Риска */
        int x1 = cx + (int)(cosf(tickRad) * (radius + 8));
        int y1 = cy + (int)(sinf(tickRad) * (radius + 8));
        int x2 = cx + (int)(cosf(tickRad) * radius);
        int y2 = cy + (int)(sinf(tickRad) * radius);
        gfx_set_color_uint32(textColor);
        gfx_line(x1, y1, x2, y2);

        /* Текст поділки */
        char buf[16];
        FormatCamValueGfx(buf, sizeof(buf), tickValue, minValue, maxValue);
        int chars = utf8_strlen(buf);
        int tw = chars * (font.glyph_width + spacing) - 2;
        
        float labelRadius = radius + 18.0f;
        int tx = cx + (int)(cosf(tickRad) * labelRadius) - tw / 2;
        int ty = cy + (int)(sinf(tickRad) * labelRadius) - font.glyph_height / 2;
        
        if (tickAngle < -90.0f || tickAngle > 90.0f) tx -= 4;
        else tx += 4;
        
        DrawTextScaled(font, tx, ty, buf, spacing, 1, textColor);
    }

    /* 6. Tooltip (підказка зверху) */
    if (MouseOverCircleGfx(cx, cy, (int)radius) && textTop && textTop[0]) {
        int pad = 6;
        char tmp[128];
        strncpy(tmp, textTop, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = '\0';

        const char *lines[5];
        int lineCount = 0;
        char *line = strtok(tmp, "\n");
        while (line && lineCount < 5) {
            lines[lineCount++] = line;
            line = strtok(NULL, "\n");
        }

        float maxW = 0;
        for (int i = 0; i < lineCount; i++) {
            int cc = utf8_strlen(lines[i]);
            float w = cc * (font.glyph_width + spacing) - 2;
            if (w > maxW) maxW = w;
        }

        float lh = font.glyph_height;
        float totalH = lineCount * lh + (lineCount > 1 ? (lineCount - 1) * 2 : 0) + pad * 2;
        int rx = cx - (int)(maxW / 2) - pad;
        int ry = cy - (int)radius - (int)totalH - pad;

        DrawRectangleFast(rx, ry, (int)maxW + 2*pad, (int)totalH, textColor);
        for (int i = 0; i < lineCount; i++) {
            DrawTextScaled(font, rx + pad, ry + pad + i * (lh + 2),
                          lines[i], spacing, 1, GetContrastColor(textColor));
        }
    }

    /* 7. Текст праворуч */
    if (textRight && textRight[0]) {
        int tw = MeasureTextGfx(font, textRight, spacing);
        int tx = cx + (int)radius + 10;
        int ty = cy - font.glyph_height / 2;
        DrawTextScaled(font, tx, ty, textRight, spacing, 1, textColor);
    }

    return changed ? 1 : 0;
}
