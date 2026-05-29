/* button_independent_gfx.c */
#include "button_independent_gfx.h"
#include "color_utils.h"
#include "graphics.h"
#include "gfx.h"

bool Gui_ButtonIndependent_GFX(int x, int y, int width, int height,
                               RasterFont font, const char *text, int spacing,
                               uint32_t colorBase, uint32_t colorText,
                               bool isActive) {
    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();
    bool mouseOver = (mx >= x && mx <= x + width && my >= y && my <= y + height);
    bool pressed = false;

    // Визначаємо колір кнопки
    uint32_t btnColor;
    if (isActive) {
        // Активна: яскравий колір
        btnColor = colorBase;
    } else {
        // Неактивна: затемнений колір (імітація Fade)
        uint8_t r = (colorBase >> 16) & 0xFF;
        uint8_t g = (colorBase >> 8) & 0xFF;
        uint8_t b = colorBase & 0xFF;
        // Затемнення до ~40% яскравості
        r = (uint8_t)(r * 0.4f);
        g = (uint8_t)(g * 0.4f);
        b = (uint8_t)(b * 0.4f);
        btnColor = (r << 16) | (g << 8) | b;
    }

    // Додаткове затемнення при наведенні/натисканні
    if (mouseOver) {
        if (gfx_is_mouse_down(GFX_MOUSE_LEFT)) {
            // Ще темніше при натисканні
            uint8_t r = (btnColor >> 16) & 0xFF;
            uint8_t g = (btnColor >> 8) & 0xFF;
            uint8_t b = btnColor & 0xFF;
            r = (uint8_t)(r * 0.8f);
            g = (uint8_t)(g * 0.8f);
            b = (uint8_t)(b * 0.8f);
            btnColor = (r << 16) | (g << 8) | b;
        }
    }

    // Контрастна обводка
    uint32_t borderColor = GetContrastColor(btnColor);
    DrawRectangleLinesFast(x - 2, y - 2, width + 4, height + 4, borderColor);

    // Тіло кнопки
    DrawRectangleFast(x, y, width, height, btnColor);

    // Якщо активна — додаємо білий контур-індикатор
    if (isActive) {
        DrawRectangleLinesFast(x - 4, y - 4, width + 8, height + 8, WHITE);
    }

    // Колір тексту (авто-контраст, якщо передано 0)
    uint32_t textColor = (colorText == 0) ? GetContrastColor(btnColor) : colorText;

    // Центрування тексту
    int charCount = utf8_strlen(text);
    int textWidth = charCount * (font.glyph_width + spacing) - spacing;
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - font.glyph_height) / 2;

    DrawTextScaled(font, textX, textY, text, spacing, 1, textColor);

    // Клік фіксується при відпусканні ЛКМ над кнопкою
    if (mouseOver && gfx_is_mouse_released(GFX_MOUSE_LEFT)) {
        pressed = true;
    }

    return pressed;
}
