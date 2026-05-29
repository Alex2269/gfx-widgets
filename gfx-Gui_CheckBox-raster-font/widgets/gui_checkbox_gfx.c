/* gui_checkbox_gfx.c */
#include "gui_checkbox_gfx.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>

/* Вимірювання ширини тексту */
static int MeasureTextGfx(RasterFont font, const char *text, int sp) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + sp) - sp;
}

/* Малювання галочки ✓ */
static void DrawCheckmarkGfx(int x, int y, int size, uint32_t color) {
    // Прості координати для галочки (3 точки)
    int p1x = x + size * 0.2f, p1y = y + size * 0.5f;
    int p2x = x + size * 0.45f, p2y = y + size * 0.75f;
    int p3x = x + size * 0.8f, p3y = y + size * 0.25f;
    
    DrawLine(p1x, p1y, p2x, p2y, color);
    DrawLine(p2x, p2y, p3x, p3y, color);
}

void Gui_CheckBox_GFX(int x, int y, int size, bool *checked,
                      RasterFont font, const char *textTop, const char *textRight,
                      uint32_t colorActive, int textSpacing) {
    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();
    bool mouseOver = (mx >= x && mx <= x + size && my >= y && my <= y + size);

    /* Колір чекбокса */
    uint32_t boxColor = (*checked) ? colorActive : ColorBrighten(colorActive, 0.25f);
    if (mouseOver) boxColor = ColorBrighten(boxColor, 1.15f);  /* +15% при наведенні */

    /* Контрастна рамка */
    uint32_t borderColor = GetContrastColor(boxColor);
    DrawRectangleLinesFast(x - 2, y - 2, size + 4, size + 4, borderColor);

    /* Тіло чекбокса */
    DrawRectangleFast(x, y, size, size, boxColor);

    /* Галочка, якщо активний */
    if (*checked) {
        DrawCheckmarkGfx(x, y, size, GetContrastColor(boxColor));
    }

    /* Колір тексту */
    uint32_t textColor = GetContrastColor(boxColor);

    /* ================= TEXT RIGHT (праворуч) ================= */
    if (textRight && textRight[0]) {
        /* Рахуємо рядки та максимальну ширину */
        char tmp[256];
        strncpy(tmp, textRight, sizeof(tmp)-1);
        tmp[sizeof(tmp)-1] = '\0';

        const char *lines[10];
        int lc = 0;
        char *line = strtok(tmp, "\n");
        while (line && lc < 10) { lines[lc++] = line; line = strtok(NULL, "\n"); }

        int maxW = 0;
        for (int i = 0; i < lc; i++) {
            int tw = MeasureTextGfx(font, lines[i], textSpacing);
            if (tw > maxW) maxW = tw;
        }

        int lh = font.glyph_height;
        int totalH = lc * lh + (lc-1)*2 + 8;  /* 2px між рядками + padding */
        int tx = x + size + 10;
        int ty = y + size/2 - totalH/2;

        /* Фон під текстом */
        uint32_t bg = ColorBrighten(colorActive, 0.9f);
        DrawRectangleFast(tx - 4, ty - 4, maxW + 8, totalH + 8, bg);
        DrawRectangleLinesFast(tx - 4, ty - 4, maxW + 8, totalH + 8, GetContrastColor(bg));

        for (int i = 0; i < lc; i++) {
            DrawTextScaled(font, tx, ty + i*(lh+2), lines[i], textSpacing, 1, textColor);
        }
    }

    /* ================= TOOLTIP (textTop при наведенні) ================= */
    if (mouseOver && textTop && textTop[0]) {
        char tmp[256];
        strncpy(tmp, textTop, sizeof(tmp)-1);
        tmp[sizeof(tmp)-1] = '\0';

        const char *lines[10];
        int lc = 0;
        char *line = strtok(tmp, "\n");
        while (line && lc < 10) { lines[lc++] = line; line = strtok(NULL, "\n"); }

        int maxW = 0;
        for (int i = 0; i < lc; i++) {
            int tw = MeasureTextGfx(font, lines[i], textSpacing);
            if (tw > maxW) maxW = tw;
        }

        int lh = font.glyph_height;
        int totalH = lc * lh + (lc-1)*2 + 8;
        int tx = x + size/2 - maxW/2;
        int ty = y - totalH - 8;

        /* Темний фон підказки */
        DrawRectangleFast(tx - 4, ty - 4, maxW + 8, totalH + 8, 0x202020);
        DrawRectangleLinesFast(tx - 4, ty - 4, maxW + 8, totalH + 8, WHITE);

        for (int i = 0; i < lc; i++) {
            DrawTextScaled(font, tx, ty + i*(lh+2), lines[i], textSpacing, 1, WHITE);
        }
    }

    /* ================= ОБРОБКА КЛІКУ ================= */
    if (mouseOver && gfx_is_mouse_released(GFX_MOUSE_LEFT)) {
        *checked = !(*checked);
    }
}
