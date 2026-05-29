/* slider_widget_gfx.c */
#include "slider_widget_gfx.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

extern int spacing;  // Відступ між символами (з main.c)

#define MAX_SLIDERS_GFX 16
#define KNOB_SIZE_GFX 10
#define CAPTURE_RADIUS_GFX 15

typedef struct {
    int x, y, w, h;
    float *value;
    float minVal, maxVal;
    bool isVertical;
    uint32_t baseColor;
    bool isActive;
    bool used;
    const char *textTop;
    const char *textRight;
} SliderGfx;

static SliderGfx slidersGfx[MAX_SLIDERS_GFX] = {0};
static int slidersCountGfx = 0;
static int activeSliderIndexGfx = -1;

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

static bool IsMouseNearKnobGfx(SliderGfx *s) {
    if (!s->used || !s->value) return false;
    float range = s->maxVal - s->minVal; if (range == 0) range = 1;
    float norm = (*s->value - s->minVal) / range;
    if (norm < 0) norm = 0; if (norm > 1) norm = 1;

    float kx, ky;
    if (s->isVertical) { kx = s->x + s->w / 2.0f; ky = s->y + (1.0f - norm) * s->h; }
    else               { kx = s->x + norm * s->w;      ky = s->y + s->h / 2.0f; }

    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    float dx = mx - kx, dy = my - ky;
    return (dx*dx + dy*dy) <= (CAPTURE_RADIUS_GFX * CAPTURE_RADIUS_GFX);
}

static void UpdateValueFromMouseGfx(SliderGfx *s) {
    if (!s->value) return;
    float norm;
    if (s->isVertical) norm = 1.0f - (float)(gfx_get_mouse_y() - s->y) / s->h;
    else               norm = (float)(gfx_get_mouse_x() - s->x) / s->w;
    if (norm < 0) norm = 0; if (norm > 1) norm = 1;
    *s->value = s->minVal + norm * (s->maxVal - s->minVal);
}

static int MeasureTextGfx(RasterFont font, const char *text, int sp) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + sp) - sp;
}

/* ================= ПУБЛІЧНІ ФУНКЦІ ================= */

void RegisterSliderGfx(int idx, int x, int y, int w, int h,
                       RasterFont font, int sp,
                       const char *tTop, const char *tRight,
                       float *val, float minV, float maxV,
                       bool vert, uint32_t col) {
    if (idx < 0 || idx >= MAX_SLIDERS_GFX) return;
    if (!slidersGfx[idx].used) {
        slidersGfx[idx].used = true;
        if (idx >= slidersCountGfx) slidersCountGfx = idx + 1;
    }
    slidersGfx[idx].x = x; slidersGfx[idx].y = y;
    slidersGfx[idx].w = w; slidersGfx[idx].h = h;
    slidersGfx[idx].value = val; slidersGfx[idx].minVal = minV; slidersGfx[idx].maxVal = maxV;
    slidersGfx[idx].isVertical = vert;
    slidersGfx[idx].baseColor = ColorBrighten(col, 0.75f);  // ✅ Запас яскравості
    slidersGfx[idx].textTop = tTop; slidersGfx[idx].textRight = tRight;
    slidersGfx[idx].isActive = false;

    UpdateSlidersAndDrawGfx(font, sp);
}

void UpdateSlidersAndDrawGfx(RasterFont font, int sp) {
    /* --- Обробка миші --- */
    if (gfx_is_mouse_pressed(GFX_MOUSE_LEFT) && activeSliderIndexGfx == -1) {
        for (int i = slidersCountGfx - 1; i >= 0; i--) {
            if (slidersGfx[i].used && IsMouseNearKnobGfx(&slidersGfx[i])) {
                for (int k = 0; k < slidersCountGfx; k++) slidersGfx[k].isActive = false;
                activeSliderIndexGfx = i;
                slidersGfx[i].isActive = true;
                break;
            }
        }
    }
    if (gfx_is_mouse_released(GFX_MOUSE_LEFT)) {
        for (int k = 0; k < slidersCountGfx; k++) slidersGfx[k].isActive = false;
        activeSliderIndexGfx = -1;
    }
    if (activeSliderIndexGfx != -1 && gfx_is_mouse_down(GFX_MOUSE_LEFT)) {
        UpdateValueFromMouseGfx(&slidersGfx[activeSliderIndexGfx]);
    }

    /* --- Малювання треків та звичайних ручок --- */
    for (int i = 0; i < slidersCountGfx; i++) {
        SliderGfx *s = &slidersGfx[i];
        if (!s->used || !s->value) continue;

        float range = s->maxVal - s->minVal; if (range == 0) range = 1;
        float norm = (*s->value - s->minVal) / range;
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;

        // ✅ Фон треку: світлий для контрасту
        DrawRectangleFast(s->x, s->y, s->w, s->h, 0xE0E0E0);
        DrawRectangleLinesFast(s->x, s->y, s->w, s->h, GetContrastColor(s->baseColor));

        uint32_t knobColor = s->baseColor;
        if (!s->isActive && IsMouseNearKnobGfx(s)) {
            knobColor = ColorBrighten(s->baseColor, 1.15f);  // Hover
        }

        // Позиція центру ручки
        float kx, ky;
        if (s->isVertical) { kx = s->x + s->w / 2.0f; ky = s->y + (1.0f - norm) * s->h; }
        else               { kx = s->x + norm * s->w;      ky = s->y + s->h / 2.0f; }

        // Малюємо прямокутну ручку
        if (s->isVertical)
            DrawRectangleFast((int)(s->x), (int)(ky - KNOB_SIZE_GFX/2), s->w, KNOB_SIZE_GFX, knobColor);
        else
            DrawRectangleFast((int)(kx - KNOB_SIZE_GFX/2), (int)(s->y), KNOB_SIZE_GFX, s->h, knobColor);
    }

    /* --- Активна ручка поверх усіх --- */
    if (activeSliderIndexGfx != -1) {
        SliderGfx *s = &slidersGfx[activeSliderIndexGfx];
        float range = s->maxVal - s->minVal; if (range == 0) range = 1;
        float norm = (*s->value - s->minVal) / range;
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;

        uint32_t activeColor = ColorBrighten(s->baseColor, 1.3f);  // Active
        float kx, ky;
        if (s->isVertical) { kx = s->x + s->w / 2.0f; ky = s->y + (1.0f - norm) * s->h; }
        else               { kx = s->x + norm * s->w;      ky = s->y + s->h / 2.0f; }

        if (s->isVertical)
            DrawRectangleFast((int)(s->x), (int)(ky - KNOB_SIZE_GFX/2), s->w, KNOB_SIZE_GFX, activeColor);
        else
            DrawRectangleFast((int)(kx - KNOB_SIZE_GFX/2), (int)(s->y), KNOB_SIZE_GFX, s->h, activeColor);
    }

    /* --- Підказки та числове значення --- */
    for (int i = slidersCountGfx - 1; i >= 0; i--) {
        SliderGfx *s = &slidersGfx[i];
        if (!s->used) continue;
        if (!s->isActive && !IsMouseNearKnobGfx(s)) continue;

        int pad = 6;
        uint32_t bg = GetContrastInvertColor(s->baseColor);
        uint32_t fg = GetContrastColor(bg);

        // textTop
        if (s->textTop && s->textTop[0]) {
            int tw = MeasureTextGfx(font, s->textTop, sp);
            int tx = s->x + s->w/2 - tw/2;
            int ty = s->y - font.glyph_height - 2*pad;
            DrawRectangleFast(tx-pad, ty-pad, tw+2*pad, font.glyph_height+2*pad, bg);
            DrawRectangleLinesFast(tx-pad, ty-pad, tw+2*pad, font.glyph_height+2*pad, fg);
            DrawTextScaled(font, tx, ty, s->textTop, sp, 1, fg);
        }

        // textRight
        if (s->textRight && s->textRight[0]) {
            int tw = MeasureTextGfx(font, s->textRight, sp);
            int tx = s->x + s->w + 10;
            int ty = s->y + s->h/2 - font.glyph_height/2;
            DrawRectangleFast(tx-pad, ty-pad, tw+2*pad, font.glyph_height+2*pad, bg);
            DrawRectangleLinesFast(tx-pad, ty-pad, tw+2*pad, font.glyph_height+2*pad, fg);
            DrawTextScaled(font, tx, ty, s->textRight, sp, 1, fg);
        }

        // Числове значення
        char valStr[16]; snprintf(valStr, sizeof(valStr), "%.2f", *s->value);
        int tw = MeasureTextGfx(font, valStr, sp);
        int tx = s->x + s->w + 12;
        int ty = s->y + s->h/2 + 20;
        DrawRectangleFast(tx-pad, ty-pad, tw+2*pad, font.glyph_height+2*pad, bg);
        DrawRectangleLinesFast(tx-pad, ty-pad, tw+2*pad, font.glyph_height+2*pad, fg);
        DrawTextScaled(font, tx, ty, valStr, sp, 1, fg);

        break; // Показуємо підказки тільки для одного слайдера
    }
}
