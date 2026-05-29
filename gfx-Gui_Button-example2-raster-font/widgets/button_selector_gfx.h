/* button_selector_gfx.h */
#ifndef BUTTON_SELECTOR_GFX_H
#define BUTTON_SELECTOR_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Кнопка вибору каналу з підсвіткою активного стану.
 * @param isActive true, якщо кнопка зараз обрана (малюється додатковий контур)
 * @return true при кліку (відпусканні ЛКМ над кнопкою)
 */
bool Gui_ButtonSelector_GFX(int x, int y, int width, int height,
                            RasterFont font, const char *text, int spacing,
                            uint32_t colorNormal, uint32_t colorHover, uint32_t colorPressed,
                            uint32_t colorText, bool isActive);

#endif // BUTTON_SELECTOR_GFX_H
