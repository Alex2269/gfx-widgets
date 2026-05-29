/* draw_vscale_gfx.h */
#ifndef DRAW_VSCALE_GFX_H
#define DRAW_VSCALE_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdint.h>

/**
 * @brief Малює вертикальну шкалу (вісь Y).
 * @param channel Номер каналу (резерв)
 * @param scale Масштаб (пікселів на одиницю)
 * @param offset_y Зсув центрального значення по Y
 * @param x, y, w, h Область малювання
 * @param font Шрифт для чисел
 * @param color Колір рисок та тексту
 */
void DrawVerticalScale_GFX(int channel, float scale, float offset_y,
                           int x, int y, int w, int h,
                           RasterFont font, uint32_t color);

#endif // DRAW_VSCALE_GFX_H
