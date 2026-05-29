/* slider_widget_circle_gfx.c */
#include "slider_widget_circle_gfx.h"
#include "color_utils.h"  // ✅ Додано для ColorBrighten/ColorHover/ColorActive
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

extern int spacing;  // Відступ між символами (з main.c)

/* ================= ГЛОБАЛЬНИЙ СТАН ================= */
static CircleSliderGfx slidersGfx[MAX_CIRCLE_SLIDERS_GFX] = {0};
static int slidersCountGfx = 0;
static int activeSliderIndexGfx = -1;

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

/* Перевірка дистанції до центру ручки */
static bool IsMouseNearCircleKnobGfx(CircleSliderGfx *s) {
    if (!s->used || !s->value) return false;

    float range = s->maxValue - s->minValue;
    if (range == 0) range = 1;
    float norm = (*s->value - s->minValue) / range;
    if (norm < 0) norm = 0; if (norm > 1) norm = 1;

    float knobX, knobY;
    if (s->isVertical) {
        knobX = s->x + s->width / 2.0f;
        knobY = s->y + (1.0f - norm) * s->height;
    } else {
        knobX = s->x + norm * s->width;
        knobY = s->y + s->height / 2.0f;
    }

    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();
    int dx = mx - (int)knobX;
    int dy = my - (int)knobY;

    return (dx*dx + dy*dy) <= (CAPTURE_RADIUS_GFX * CAPTURE_RADIUS_GFX);
}

/* Оновлення значення з позиції миші */
static void UpdateValueFromMouseCircleGfx(CircleSliderGfx *s) {
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

/* Малювання заповненого кола (оптимізовано лініями) */
static void DrawFilledCircleGfx(int cx, int cy, int radius, uint32_t color) {
    if (radius <= 0) return;
    gfx_set_color_uint32(color);
    for (int y = -radius; y <= radius; y++) {
        int dx = (int)sqrt((float)radius*radius - (float)y*y);
        gfx_line(cx - dx, cy + y, cx + dx, cy + y);
    }
}

/* Вимірювання ширини тексту */
static int MeasureTextGfx(RasterFont font, const char *text, int sp) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + sp) - sp;
}

/* ================= ПУБЛІЧНІ ФУНКЦІЇ ================= */

void RegisterCircleKnobSliderGfx(int idx, int x, int y, int w, int h,
                                 float *val, float minV, float maxV,
                                 bool vert, uint32_t col,
                                 const char *tTop, const char *tRight) {
    if (idx < 0 || idx >= MAX_CIRCLE_SLIDERS_GFX) return;

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
    slidersGfx[idx].baseColor = ColorBrighten(col, 0.75f);
    slidersGfx[idx].textTop = tTop;
    slidersGfx[idx].textRight = tRight;
    slidersGfx[idx].isActive = false;
}

void UpdateCircleKnobSlidersAndDrawGfx(RasterFont font, int sp) {
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
        UpdateValueFromMouseCircleGfx(&slidersGfx[activeSliderIndexGfx]);
    }

    /* --- Малювання треків та звичайних ручок --- */
    for (int i = 0; i < slidersCountGfx; i++) {
        CircleSliderGfx *s = &slidersGfx[i];
        if (!s->used || !s->value) continue;

        float range = s->maxValue - s->minValue;
        if (range == 0) range = 1;
        float norm = (*s->value - s->minValue) / range;
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;

        // ✅ 1. Фон треку: нейтральний світло-сірий для максимального контрасту
        DrawRectangleFast(s->x, s->y, s->width, s->height, ColorBrighten(0xE8E8E8, 0.50f));
        DrawRectangleLinesFast(s->x, s->y, s->width, s->height, GetContrastColor(s->baseColor));

        // ✅ 2. Колір ручки через ColorBrighten (надійна зміна яскравості)
        uint32_t knobColor = s->baseColor;
        if (!s->isActive && IsMouseNearCircleKnobGfx(s)) {
            knobColor = ColorBrighten(s->baseColor, 1.15f);  // +15% яскравості — завжди видно!
        }

        // 3. Позиція ручки
        int knobX, knobY;
        if (s->isVertical) {
            knobX = s->x + s->width / 2;
            knobY = s->y + (int)((1.0f - norm) * s->height);
        } else {
            knobX = s->x + (int)(norm * s->width);
            knobY = s->y + s->height / 2;
        }

        DrawFilledCircleGfx(knobX, knobY, SLIDER_KNOB_RADIUS_GFX, knobColor);
    }

    /* --- Активна ручка поверх усіх --- */
    if (activeSliderIndexGfx != -1) {
        CircleSliderGfx *s = &slidersGfx[activeSliderIndexGfx];
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
        DrawFilledCircleGfx(knobX, knobY, SLIDER_KNOB_RADIUS_GFX, activeColor);
    }

    /* --- Підказки та числове значення --- */
    for (int i = slidersCountGfx - 1; i >= 0; i--) {
        CircleSliderGfx *s = &slidersGfx[i];
        if (!s->used) continue;

        if (s->isActive || IsMouseNearCircleKnobGfx(s)) {
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

                for (int li = 0; li < lc; li++)
                    DrawTextScaled(font, tx, ty + li*(lh+2), lines[li], sp, 1, GetContrastColor(bg));
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

                for (int li = 0; li < lc; li++)
                    DrawTextScaled(font, tx, ty + li*(lh+2), lines[li], sp, 1, GetContrastColor(bg));
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
