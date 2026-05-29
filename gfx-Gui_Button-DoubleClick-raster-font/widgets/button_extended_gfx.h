// button_extended_gfx.h
#ifndef BUTTON_EXTENDED_GFX_H
#define BUTTON_EXTENDED_GFX_H

#include "gfx.h"
#include <stdbool.h>
#include <stdint.h>
#include "glyphs.h"
#include "color_utils.h"

// Структура стану кнопки для long press і double click
typedef struct {
    double lastClickTime;       // Час останнього кліку
    double longPressStartTime;  // Час початку тривалого натискання
    bool isPressed;             // Чи натиснута кнопка зараз
    bool longPressTriggered;    // Чи було уже тригернуто тривале натискання
} ButtonStateGfx;

/**
 * Розширена кнопка для gfx з підтримкою:
 * - одинарного кліку
 * - подвійного кліку (double click)
 * - тривалого натискання (long press)
 * 
 * @param x, y - координати лівого верхнього кута кнопки
 * @param width, height - розміри кнопки
 * @param font - шрифт для тексту
 * @param text - текст кнопки
 * @param spacing - відступ між символами
 * @param colorNormal - колір кнопки в нормальному стані (0xRRGGBB)
 * @param colorHover - колір при наведенні
 * @param colorPressed - колір при натисканні
 * @param colorText - колір тексту (0 для авто-контрасту)
 * @param state - вказівник на структуру стану кнопки
 * @param outLongPress - вихідний параметр: true при long press
 * @param outDoubleClick - вихідний параметр: true при double click
 * @return true при одинарному кліку (відпусканні)
 */
bool Gui_ButtonExtended_GFX(int x, int y, int width, int height,
                            RasterFont font, const char *text, int spacing,
                            uint32_t colorNormal, uint32_t colorHover, uint32_t colorPressed,
                            uint32_t colorText, ButtonStateGfx *state,
                            bool *outLongPress, bool *outDoubleClick);

#endif // BUTTON_EXTENDED_GFX_H
