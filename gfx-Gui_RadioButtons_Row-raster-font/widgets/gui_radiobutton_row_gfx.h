/* gui_radiobutton_row_gfx.h */
#ifndef GUI_RADIOBUTTON_ROW_GFX_H
#define GUI_RADIOBUTTON_ROW_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Малює ряд квадратних радіокнопок для gfx.
 * @param boundsX, boundsY Координати лівого верхнього кута групи
 * @param items Масив рядків-назв кнопок
 * @param itemCount Кількість кнопок у ряді
 * @param currentIndex Індекс поточного вибраного елемента
 * @param colorActive Базовий колір активної кнопки (0xRRGGBB)
 * @param buttonSize Розмір квадратної кнопки (пікселі)
 * @param spacing Відступ між кнопками (пікселі)
 * @param font Шрифт для тексту кнопок
 * @param textSpacing Відступ між символами тексту
 * @return Новий індекс вибраної кнопки (або поточний, якщо змін не було)
 */
int Gui_RadioButtons_Row_GFX(int boundsX, int boundsY,
                             const char **items, int itemCount,
                             int currentIndex, uint32_t colorActive,
                             int buttonSize, int spacing,
                             RasterFont font, int textSpacing);

#endif // GUI_RADIOBUTTON_ROW_GFX_H
