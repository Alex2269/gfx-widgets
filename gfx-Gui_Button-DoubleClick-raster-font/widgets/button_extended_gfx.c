// button_extended_gfx.c
#include "button_extended_gfx.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// Константи часу
static const double LONG_PRESS_DURATION = 0.5;     // 0.5 сек для long press
static const double DOUBLE_CLICK_INTERVAL = 0.3;   // 0.3 сек між кліками для double click

// Вимірювання ширини тексту
static int MeasureTextWidthGfx(RasterFont font, const char *text, int spacing) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + spacing) - spacing;
}

bool Gui_ButtonExtended_GFX(int x, int y, int width, int height,
                            RasterFont font, const char *text, int spacing,
                            uint32_t colorNormal, uint32_t colorHover, uint32_t colorPressed,
                            uint32_t colorText, ButtonStateGfx *state,
                            bool *outLongPress, bool *outDoubleClick) {
    // Отримуємо позицію миші
    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();
    
    // Перевірка чи миша над кнопкою
    bool mouseOver = (mx >= x && mx <= x + width && my >= y && my <= y + height);
    
    bool pressed = false;
    double currentTime = GetTimeGfx();
    
    // Скидаємо вихідні прапорці
    if (outLongPress) *outLongPress = false;
    if (outDoubleClick) *outDoubleClick = false;
    
    // Визначаємо колір кнопки
    uint32_t btnColor = colorNormal;
    if (mouseOver) btnColor = colorHover;
    if (mouseOver && gfx_is_mouse_down(GFX_MOUSE_LEFT)) btnColor = colorPressed;
    
    // Малюємо рамку
    uint32_t borderColor = GetContrastColor(btnColor);
    int borderThickness = 2;
    DrawRectangleLinesFast(x - borderThickness, y - borderThickness,
                          width + 2*borderThickness, height + 2*borderThickness,
                          borderColor);
    
    // Малюємо кнопку
    DrawRectangleFast(x, y, width, height, btnColor);
    
    // Визначаємо колір тексту
    uint32_t textColor = colorText;
    if (colorText == 0) {
        textColor = GetContrastColor(btnColor);
    }
    
    // Центруємо текст
    int textWidth = MeasureTextWidthGfx(font, text, spacing);
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - font.glyph_height) / 2;
    
    DrawTextScaled(font, textX, textY, text, spacing, 1, textColor);
    
    // Обробка натискань
    if (mouseOver) {
        if (gfx_is_mouse_down(GFX_MOUSE_LEFT)) {
            if (!state->isPressed) {
                // Кнопка тільки-но натиснута
                state->isPressed = true;
                state->longPressStartTime = currentTime;
                state->longPressTriggered = false;
            } else {
                // Тримаємо кнопку - перевіряємо long press
                if (!state->longPressTriggered && 
                    (currentTime - state->longPressStartTime > LONG_PRESS_DURATION)) {
                    if (outLongPress) *outLongPress = true;
                    state->longPressTriggered = true;
                }
            }
        } else {
            if (state->isPressed) {
                // Кнопка відпущена - це клік
                pressed = true;
                
                // Перевірка на double click
                if ((currentTime - state->lastClickTime) < DOUBLE_CLICK_INTERVAL) {
                    if (outDoubleClick) *outDoubleClick = true;
                }
                
                state->lastClickTime = currentTime;
                state->isPressed = false;
                state->longPressTriggered = false;
            }
        }
    } else {
        // Миша вийшла з кнопки - скидаємо стан
        state->isPressed = false;
        state->longPressTriggered = false;
    }
    
    return pressed;
}
