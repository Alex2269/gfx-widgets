/* button_gfx.c */
#include "button_gfx.h"
#include "color_utils.h"
#include "graphics.h"
#include "gfx.h"

bool Gui_Button_GFX(int x, int y, int width, int height,
                    RasterFont font, const char *text, int spacing,
                    uint32_t colorNormal, uint32_t colorHover, uint32_t colorPressed,
                    uint32_t colorText) {
    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();
    bool mouseOver = (mx >= x && mx <= x + width && my >= y && my <= y + height);
    bool pressed = false;

    // Визначаємо колір залежно від стану
    uint32_t btnColor = colorNormal;
    if (mouseOver) btnColor = colorHover;
    if (mouseOver && gfx_is_mouse_down(GFX_MOUSE_LEFT)) btnColor = colorPressed;

    // Малюємо обводку (комірець)
    uint32_t borderColor = GetContrastColor(btnColor);
    DrawRectangleLinesFast(x - 2, y - 2, width + 4, height + 4, borderColor);

    // Малюємо тіло кнопки
    DrawRectangleFast(x, y, width, height, btnColor);

    // Авто-підбір кольору тексту, якщо передано 0
    uint32_t textColor = (colorText == 0) ? GetContrastColor(btnColor) : colorText;

    // Центрування тексту
    int charCount = utf8_strlen(text);
    int textWidth = charCount * (font.glyph_width + spacing) - spacing;
    int textHeight = font.glyph_height;
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - textHeight) / 2;

    DrawTextScaled(font, textX, textY, text, spacing, 1, textColor);

    // Клік фіксується при відпусканні ЛКМ над кнопкою
    if (mouseOver && gfx_is_mouse_released(GFX_MOUSE_LEFT)) {
        pressed = true;
    }

    return pressed;
}
