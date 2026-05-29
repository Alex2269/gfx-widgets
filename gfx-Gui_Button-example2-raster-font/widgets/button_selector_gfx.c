/* button_selector_gfx.c */
#include "button_selector_gfx.h"
#include "color_utils.h"
#include "graphics.h"
#include "gfx.h"

bool Gui_ButtonSelector_GFX(int x, int y, int width, int height,
                            RasterFont font, const char *text, int spacing,
                            uint32_t colorNormal, uint32_t colorHover, uint32_t colorPressed,
                            uint32_t colorText, bool isActive) {
    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();
    bool mouseOver = (mx >= x && mx <= x + width && my >= y && my <= y + height);
    bool pressed = false;

    // Визначення кольору фону
    uint32_t btnColor = colorNormal;
    if (mouseOver) btnColor = colorHover;
    if (mouseOver && gfx_is_mouse_down(GFX_MOUSE_LEFT)) btnColor = colorPressed;

    // Зовнішній контур
    uint32_t borderColor = GetContrastColor(btnColor);
    DrawRectangleLinesFast(x - 2, y - 2, width + 4, height + 4, borderColor);

    // Якщо кнопка активна — додаємо яскраву обводку-індикатор
    if (isActive) {
        DrawRectangleLinesFast(x - 4, y - 4, width + 8, height + 8, WHITE);
    }

    // Тіло кнопки
    DrawRectangleFast(x, y, width, height, btnColor);

    // Текст (авто-контраст, якщо передано 0)
    uint32_t textColor = (colorText == 0) ? GetContrastColor(btnColor) : colorText;
    int charCount = utf8_strlen(text);
    int textWidth = charCount * (font.glyph_width + spacing) - spacing;
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - font.glyph_height) / 2;

    DrawTextScaled(font, textX, textY, text, spacing, 1, textColor);

    // Клік фіксується при відпусканні ЛКМ
    if (mouseOver && gfx_is_mouse_released(GFX_MOUSE_LEFT)) {
        pressed = true;
    }

    return pressed;
}
