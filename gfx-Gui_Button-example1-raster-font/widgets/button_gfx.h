/* button_gfx.h */
#ifndef BUTTON_GFX_H
#define BUTTON_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Проста кнопка для gfx з підтримкою станів наведення та натискання.
 * @return true при одинарному кліку (відпусканні ЛКМ над кнопкою).
 */
bool Gui_Button_GFX(int x, int y, int width, int height,
                    RasterFont font, const char *text, int spacing,
                    uint32_t colorNormal, uint32_t colorHover, uint32_t colorPressed,
                    uint32_t colorText);

#endif // BUTTON_GFX_H
