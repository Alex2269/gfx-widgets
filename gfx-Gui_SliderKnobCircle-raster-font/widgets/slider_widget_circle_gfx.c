/* slider_widget_circle_gfx.c */
#include "slider_widget_circle_gfx.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

extern int spacing;  // Відступ між символами (з main.c)

/* ================= ГЛОБАЛЬНИЙ СТАН ================= */
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
} CircleSliderGfx;

static CircleSliderGfx slidersGfx[MAX_CIRCLE_SLIDERS_GFX] = {0};
static int slidersCountGfx = 0;
static int activeSliderIndexGfx = -1;

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

static bool IsMouseNearCircleKnobGfx(CircleSliderGfx *s) {
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

static void UpdateValueFromMouseGfx(CircleSliderGfx *s) {
    if (!s->value) return;
    float norm;
    if (s->isVertical) norm = 1.0f - (float)(gfx_get_mouse_y() - s->y) / s->h;
    else               norm = (float)(gfx_get_mouse_x() - s->x) / s->w;
    if (norm < 0) norm = 0; if (norm > 1) norm = 1;
    *s->value = s->minVal + norm * (s->maxVal - s->minVal);
}

static void DrawFilledCircleGfx(int cx, int cy, int r, uint32_t color) {
    if (r <= 0) return;
    gfx_set_color_uint32(color);
    for (int y = -r; y <= r; y++) {
        int dx = (int)sqrt((float)r*r - (float)y*y);
        gfx_line(cx - dx, cy + y, cx + dx, cy + y);
    }
}

static int MeasureTextGfx(RasterFont font, const char *text, int sp) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + sp) - sp;
}

/* ================= ПУБЛІЧНІ ФУНКЦІЇ ================= */

void Gui_SliderKnobCircle_GFX(int idx, int x, int y, int w, int h,
                              RasterFont font, int sp,
                              const char *tTop, const char *tRight,
                              float *val, float minV, float maxV,
                              bool vert, uint32_t col) {
    if (idx < 0 || idx >= MAX_CIRCLE_SLIDERS_GFX) return;
    if (!slidersGfx[idx].used) {
        slidersGfx[idx].used = true;
        if (idx >= slidersCountGfx) slidersCountGfx = idx + 1;
    }
    slidersGfx[idx].x = x; slidersGfx[idx].y = y;
    slidersGfx[idx].w = w; slidersGfx[idx].h = h;
    slidersGfx[idx].value = val; slidersGfx[idx].minVal = minV; slidersGfx[idx].maxVal = maxV;
    slidersGfx[idx].isVertical = vert;
    slidersGfx[idx].baseColor = ColorBrighten(col, 0.75f);  // ✅ Приглушуємо для запасу яскравості
    slidersGfx[idx].textTop = tTop; slidersGfx[idx].textRight = tRight;
    slidersGfx[idx].isActive = false;

    UpdateCircleKnobSlidersAndDrawGfx(font, sp);
}

void UpdateCircleKnobSlidersAndDrawGfx(RasterFont font, int sp) {
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();

    /* --- Обробка миші --- */
    if (gfx_is_mouse_pressed(GFX_MOUSE_LEFT) && activeSliderIndexGfx == -1) {
        for (int i = slidersCountGfx - 1; i >= 0; i--) {
            if (slidersGfx[i].used && IsMouseNearCircleKnobGfx(&slidersGfx[i])) {
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
        CircleSliderGfx *s = &slidersGfx[i];
        if (!s->used || !s->value) continue;

        float range = s->maxVal - s->minVal; if (range == 0) range = 1;
        float norm = (*s->value - s->minVal) / range;
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;

        DrawRectangleFast(s->x, s->y, s->w, s->h, ColorBrighten(s->baseColor, 0.2f));
        DrawRectangleLinesFast(s->x, s->y, s->w, s->h, GetContrastColor(s->baseColor));

        uint32_t knobColor = s->baseColor;
        if (!s->isActive && IsMouseNearCircleKnobGfx(s)) {
            knobColor = ColorBrighten(s->baseColor, 1.15f);  // Hover
        }

        float kx, ky;
        if (s->isVertical) { kx = s->x + s->w / 2.0f; ky = s->y + (1.0f - norm) * s->h; }
        else               { kx = s->x + norm * s->w;      ky = s->y + s->h / 2.0f; }

        DrawFilledCircleGfx((int)kx, (int)ky, SLIDER_KNOB_RADIUS_GFX, knobColor);
    }

    /* --- Активна ручка поверх усіх --- */
    if (activeSliderIndexGfx != -1) {
        CircleSliderGfx *s = &slidersGfx[activeSliderIndexGfx];
        float range = s->maxVal - s->minVal; if (range == 0) range = 1;
        float norm = (*s->value - s->minVal) / range;
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;

        uint32_t activeColor = ColorBrighten(s->baseColor, 1.3f);  // Active
        float kx, ky;
        if (s->isVertical) { kx = s->x + s->w / 2.0f; ky = s->y + (1.0f - norm) * s->h; }
        else               { kx = s->x + norm * s->w;      ky = s->y + s->h / 2.0f; }
        DrawFilledCircleGfx((int)kx, (int)ky, SLIDER_KNOB_RADIUS_GFX, activeColor);
    }

    /* --- Підказки та числове значення --- */
    for (int i = slidersCountGfx - 1; i >= 0; i--) {
        CircleSliderGfx *s = &slidersGfx[i];
        if (!s->used) continue;
        if (!s->isActive && !IsMouseNearCircleKnobGfx(s)) continue;

        int pad = 6;
        uint32_t bg = GetContrastInvertColor(s->baseColor);
        uint32_t fg = GetContrastColor(bg);

        // textTop
        if (s->textTop && s->textTop[0]) {
            char tmp[256]; strncpy(tmp, s->textTop, sizeof(tmp)-1); tmp[sizeof(tmp)-1] = '\0';
            const char *lines[10]; int lc = 0;
            char *line = strtok(tmp, "\n");
            while (line && lc < 10) { lines[lc++] = line; line = strtok(NULL, "\n"); }
            int maxW = 0;
            for (int li = 0; li < lc; li++) { int tw = MeasureTextGfx(font, lines[li], sp); if (tw > maxW) maxW = tw; }
            int lh = font.glyph_height;
            int totalH = lc * lh + (lc-1)*2 + 2*pad;
            int tx = s->x + s->w/2 - maxW/2;
            int ty = s->y - totalH - 8;
            DrawRectangleFast(tx-pad, ty-pad, maxW+2*pad, totalH+2*pad, bg);
            DrawRectangleLinesFast(tx-pad, ty-pad, maxW+2*pad, totalH+2*pad, fg);
            for (int li = 0; li < lc; li++) DrawTextScaled(font, tx, ty+li*(lh+2), lines[li], sp, 1, fg);
        }

        // textRight
        if (s->textRight && s->textRight[0]) {
            char tmp[256]; strncpy(tmp, s->textRight, sizeof(tmp)-1); tmp[sizeof(tmp)-1] = '\0';
            const char *lines[10]; int lc = 0;
            char *line = strtok(tmp, "\n");
            while (line && lc < 10) { lines[lc++] = line; line = strtok(NULL, "\n"); }
            int maxW = 0;
            for (int li = 0; li < lc; li++) { int tw = MeasureTextGfx(font, lines[li], sp); if (tw > maxW) maxW = tw; }
            int lh = font.glyph_height;
            int totalH = lc * lh + (lc-1)*2 + 2*pad;
            int tx = s->x + s->w + 10;
            int ty = s->y + s->h/2 - totalH/2;
            DrawRectangleFast(tx-pad, ty-pad, maxW+2*pad, totalH+2*pad, bg);
            DrawRectangleLinesFast(tx-pad, ty-pad, maxW+2*pad, totalH+2*pad, fg);
            for (int li = 0; li < lc; li++) DrawTextScaled(font, tx, ty+li*(lh+2), lines[li], sp, 1, fg);
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
