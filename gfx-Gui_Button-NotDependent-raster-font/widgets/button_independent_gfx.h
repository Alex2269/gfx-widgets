/* button_independent_gfx.h */
#ifndef BUTTON_INDEPENDENT_GFX_H
#define BUTTON_INDEPENDENT_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Незалежна кнопка з toggle-станом для gfx.
 * @param isActive true, якщо кнопка зараз "увімкнена" (малюється яскравіше)
 * @return true при кліку (відпусканні ЛКМ над кнопкою)
 */
bool Gui_ButtonIndependent_GFX(int x, int y, int width, int height,
                               RasterFont font, const char *text, int spacing,
                               uint32_t colorBase, uint32_t colorText,
                               bool isActive);

#endif // BUTTON_INDEPENDENT_GFX_H
