/* gui_checkbox_gfx.h */
#ifndef GUI_CHECKBOX_GFX_H
#define GUI_CHECKBOX_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include "color_utils.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Малює чекбокс із текстом праворуч і підказкою зверху для gfx.
 * @param x, y Координати лівого верхнього кута чекбокса
 * @param size Розмір квадратного чекбокса (пікселі)
 * @param checked Вказівник на стан (true = встановлено)
 * @param font Шрифт для тексту
 * @param textTop Підказка зверху при наведенні (може бути багаторядковою, через \n)
 * @param textRight Текст праворуч від чекбокса (може бути багаторядковим)
 * @param colorActive Базовий колір активного чекбокса (0xRRGGBB)
 * @param textSpacing Відступ між символами тексту
 */
void Gui_CheckBox_GFX(int x, int y, int size, bool *checked,
                      RasterFont font, const char *textTop, const char *textRight,
                      uint32_t colorActive, int textSpacing);

#endif // GUI_CHECKBOX_GFX_H
