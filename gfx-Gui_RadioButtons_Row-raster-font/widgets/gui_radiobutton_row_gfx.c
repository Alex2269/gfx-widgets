/* gui_radiobutton_row_gfx.c */
#include "gui_radiobutton_row_gfx.h"
#include "color_utils.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>

/* Внутрішня функція квадратної кнопки для gfx */
static bool Gui_Square_Button_GFX(int x, int y, int size, RasterFont font,
                                   const char *text, int textSpacing,
                                   uint32_t colorNormal, uint32_t colorHover,
                                   uint32_t colorPressed, uint32_t colorText) {
    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();
    bool mouseOver = (mx >= x && mx <= x + size && my >= y && my <= y + size);
    bool pressed = false;

    /* Визначаємо колір кнопки */
    uint32_t btnColor = colorNormal;
    if (mouseOver) btnColor = colorHover;
    if (mouseOver && gfx_is_mouse_down(GFX_MOUSE_LEFT)) btnColor = colorPressed;

    /* Контур кнопки (комірець) */
    uint32_t borderColor = GetContrastColor(btnColor);
    DrawRectangleLinesFast(x - 2, y - 2, size + 4, size + 4, borderColor);

    /* Тіло кнопки */
    DrawRectangleFast(x, y, size, size, btnColor);

    /* Колір тексту (авто-контраст, якщо передано 0) */
    uint32_t textColor = (colorText == 0) ? GetContrastColor(btnColor) : colorText;

    /* Центрування тексту */
    int charCount = utf8_strlen(text);
    int textWidth = charCount * (font.glyph_width + textSpacing) - textSpacing;
    int textX = x + (size - textWidth) / 2;
    int textY = y + (size - font.glyph_height) / 2;

    DrawTextScaled(font, textX, textY, text, textSpacing, 1, textColor);

    /* Клік фіксується при відпусканні ЛКМ */
    if (mouseOver && gfx_is_mouse_released(GFX_MOUSE_LEFT)) {
        pressed = true;
    }

    return pressed;
}

int Gui_RadioButtons_Row_GFX(int boundsX, int boundsY,
                             const char **items, int itemCount,
                             int currentIndex, uint32_t colorActive,
                             int buttonSize, int spacing,
                             RasterFont font, int textSpacing) {
    int selectedIndex = currentIndex;

    for (int i = 0; i < itemCount; i++) {
        int btnX = boundsX + i * (buttonSize + spacing);
        int btnY = boundsY;

        /* Колір кнопки: активна — яскрава, інші — приглушені */
        uint32_t btnColor = (i == currentIndex)
                          ? colorActive
                          : ColorBrighten(colorActive, 0.25f);  /* 25% яскравості */

        /* Клік змінює вибір */
        if (Gui_Square_Button_GFX(btnX, btnY, buttonSize, font, items[i], textSpacing,
                                   btnColor,
                                   ColorBrighten(btnColor, 1.15f),   /* Hover: +15% */
                                   ColorBrighten(btnColor, 0.85f),   /* Pressed: -15% */
                                   0)) {  /* 0 = авто-контраст тексту */
            selectedIndex = i;
        }
    }

    return selectedIndex;
}
