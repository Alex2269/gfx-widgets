/* slider_knob_diamond_gfx.c */
#include "slider_knob_diamond_gfx.h"
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
} DiamondSliderGfx;

static DiamondSliderGfx slidersGfx[MAX_DIAMOND_SLIDERS_GFX] = {0};
static int slidersCountGfx = 0;
static int activeSliderIndexGfx = -1;

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

/* ✅ Малювання ромба з 3D-ефектом (світло-тінь) */
static void DrawDiamondKnob_3D_Gfx(int cx, int cy, int size, uint32_t color) {
    if (size <= 0) return;

    // Вершини
    int top_y = cy - size;
    int right_x = cx + size;
    int bottom_y = cy + size;
    int left_x = cx - size;

    // Кольори для імітації освітлення (зліва-зверху світло, справа-знизу тінь)
    uint32_t c_top_left   = ColorBrighten(ChangeSaturation(color, 1.2f), 1.15f); // Найяскравіший
    uint32_t c_top_right  = ColorBrighten(ChangeSaturation(color, 1.0f), 1.0f);  // Нормальний
    uint32_t c_bot_left   = ColorBrighten(ChangeSaturation(color, 0.9f), 0.85f); // Півтінь
    uint32_t c_bot_right  = ColorBrighten(ChangeSaturation(color, 0.7f), 0.7f);  // Тінь

    // ✅ ВИПРАВЛЕНО: Всі трикутники у контр-годинниковому порядку!

    // 1. Верх-Ліво (Світло)
    DrawTriangle(cx, top_y, left_x, cy, cx, cy, c_top_left);

    // 2. Верх-Право (Світлий)
    DrawTriangle(cx, top_y, cx, cy, right_x, cy, c_top_right);

    // 3. Низ-Ліво (Перехід) — ✅ ВИПРАВЛЕНО ПОРЯДОК!
    // Було: (cx, bottom_y, cx, cy, left_x, cy) — неправильний порядок
    // Стало: (left_x, cy, cx, cy, cx, bottom_y) — правильний контр-годинниковий
    DrawTriangle(left_x, cy, cx, cy, cx, bottom_y, c_bot_left);

    // 4. Низ-Право (Тінь) — ✅ ВИПРАВЛЕНО ПОРЯДОК!
    // Було: (cx, bottom_y, right_x, cy, cx, cy) — неправильний порядок
    // Стало: (cx, bottom_y, cx, cy, right_x, cy) — правильний контр-годинниковий
    DrawTriangle(cx, bottom_y, cx, cy, right_x, cy, c_bot_right);

    // Контур (чорний)
    gfx_set_color_uint32(BLACK);
    gfx_line(cx, top_y, right_x, cy);
    gfx_line(right_x, cy, cx, bottom_y);
    gfx_line(cx, bottom_y, left_x, cy);
    gfx_line(left_x, cy, cx, top_y);

    // Блик (білий піксель зверху)
    gfx_set_color_uint32(WHITE);
    gfx_point(cx, top_y + 1);
}

/* ✅ Перевірка попадання миші в ромб (Манхеттенська відстань) */
static bool IsMouseNearDiamondKnobGfx(DiamondSliderGfx *s) {
    if (!s->used || !s->value) return false;
    
    float range = s->maxVal - s->minVal; if (range == 0) range = 1;
    float norm = (*s->value - s->minVal) / range;
    if (norm < 0) norm = 0; if (norm > 1) norm = 1;

    float kx, ky;
    if (s->isVertical) { kx = s->x + s->w / 2.0f; ky = s->y + (1.0f - norm) * s->h; }
    else               { kx = s->x + norm * s->w;      ky = s->y + s->h / 2.0f; }

    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    float dx = fabsf(mx - kx);
    float dy = fabsf(my - ky);
    
    // Манхеттенська відстань: |dx| + |dy| <= радіус + паддінг
    return (dx + dy) <= (KNOB_DIAMOND_SIZE_GFX + CAPTURE_PAD_GFX);
}

static void UpdateValueFromMouseGfx(DiamondSliderGfx *s) {
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

/* ================= ПУБЛІЧНІ ФУНКЦІЇ ================= */

void Gui_SliderKnobDiamond_GFX(int idx, int x, int y, int w, int h,
                               RasterFont font, int sp,
                               const char *tTop, const char *tRight,
                               float *val, float minV, float maxV,
                               bool vert, uint32_t col) {
    if (idx < 0 || idx >= MAX_DIAMOND_SLIDERS_GFX) return;
    if (!slidersGfx[idx].used) {
        slidersGfx[idx].used = true;
        if (idx >= slidersCountGfx) slidersCountGfx = idx + 1;
    }
    slidersGfx[idx].x = x; slidersGfx[idx].y = y;
    slidersGfx[idx].w = w; slidersGfx[idx].h = h;
    slidersGfx[idx].value = val; slidersGfx[idx].minVal = minV; slidersGfx[idx].maxVal = maxV;
    slidersGfx[idx].isVertical = vert;
    slidersGfx[idx].baseColor = ColorBrighten(col, 0.75f);  // ✅ Запас яскравості для 3D ефекту
    slidersGfx[idx].textTop = tTop; slidersGfx[idx].textRight = tRight;
    slidersGfx[idx].isActive = false;

    UpdateDiamondKnobSlidersAndDrawGfx(font, sp);
}

void UpdateDiamondKnobSlidersAndDrawGfx(RasterFont font, int sp) {
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();

    /* --- Обробка миші --- */
    if (gfx_is_mouse_pressed(GFX_MOUSE_LEFT) && activeSliderIndexGfx == -1) {
        for (int i = slidersCountGfx - 1; i >= 0; i--) {
            if (slidersGfx[i].used && IsMouseNearDiamondKnobGfx(&slidersGfx[i])) {
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
        DiamondSliderGfx *s = &slidersGfx[i];
        if (!s->used || !s->value) continue;

        float range = s->maxVal - s->minVal; if (range == 0) range = 1;
        float norm = (*s->value - s->minVal) / range;
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;

        // Фон треку: світлий для контрасту
        DrawRectangleFast(s->x, s->y, s->w, s->h, 0xE0E0E0);
        DrawRectangleLinesFast(s->x, s->y, s->w, s->h, GetContrastColor(s->baseColor));

        // Для неактивних ручок використовуємо простіший колір або той самий 3D
        // Тут використовуємо той самий 3D метод для консистентності, але можна спростити
        float kx, ky;
        if (s->isVertical) { kx = s->x + s->w / 2.0f; ky = s->y + (1.0f - norm) * s->h; }
        else               { kx = s->x + norm * s->w;      ky = s->y + s->h / 2.0f; }

        // Якщо наведено мишею, трохи підсвічуємо базовий колір перед передачею в 3D
        uint32_t knobBase = s->baseColor;
        if (!s->isActive && IsMouseNearDiamondKnobGfx(s)) {
            knobBase = ColorBrighten(s->baseColor, 1.1f);
        }
        
        DrawDiamondKnob_3D_Gfx((int)kx, (int)ky, KNOB_DIAMOND_SIZE_GFX, knobBase);
    }

    /* --- Активна ручка поверх усіх --- */
    if (activeSliderIndexGfx != -1) {
        DiamondSliderGfx *s = &slidersGfx[activeSliderIndexGfx];
        float range = s->maxVal - s->minVal; if (range == 0) range = 1;
        float norm = (*s->value - s->minVal) / range;
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;

        // Активна ручка яскравіша
        uint32_t activeBase = ColorBrighten(s->baseColor, 1.2f);
        
        float kx, ky;
        if (s->isVertical) { kx = s->x + s->w / 2.0f; ky = s->y + (1.0f - norm) * s->h; }
        else               { kx = s->x + norm * s->w;      ky = s->y + s->h / 2.0f; }

        DrawDiamondKnob_3D_Gfx((int)kx, (int)ky, KNOB_DIAMOND_SIZE_GFX, activeBase);
    }

    /* --- Підказки та числове значення --- */
    for (int i = slidersCountGfx - 1; i >= 0; i--) {
        DiamondSliderGfx *s = &slidersGfx[i];
        if (!s->used) continue;
        if (!s->isActive && !IsMouseNearDiamondKnobGfx(s)) continue;

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
