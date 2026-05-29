/* draw_hscale_gfx.h */
#ifndef DRAW_HSCALE_GFX_H
#define DRAW_HSCALE_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdint.h>

/**
 * @brief Малює горизонтальну шкалу (вісь X).
 * @param channel Номер каналу (резерв)
 * @param scale Масштаб (пікселів на одиницю)
 * @param offset_x Зсув центрального значення по X
 * @param x, y, w, h Область малювання
 * @param font Шрифт для чисел
 * @param color Колір рисок та тексту
 */
void DrawHorizontalScale_GFX(int channel, float scale, float offset_x,
                             int x, int y, int w, int h,
                             RasterFont font, uint32_t color);

#endif // DRAW_HSCALE_GFX_H
