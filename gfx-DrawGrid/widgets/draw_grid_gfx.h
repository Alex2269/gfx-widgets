/* draw_grid_gfx.h */
#ifndef DRAW_GRID_GFX_H
#define DRAW_GRID_GFX_H

#include "gfx.h"
#include "graphics.h"
#include <stdint.h>

/**
 * @brief Малює один шар точок сітки.
 * @param startX, endX Початок і кінець по X.
 * @param stepX Крок по X.
 * @param startY, endY Початок і кінець по Y.
 * @param stepY Крок по Y.
 * @param color Колір точок (0xRRGGBB).
 */
void draw_grid_layer_gfx(int startX, int endX, int stepX,
                         int startY, int endY, int stepY,
                         uint32_t color);

/**
 * @brief Малює повну сітку осцилоскопа з центрованістю.
 * @param screenWidth, screenHeight Розміри області малювання.
 * @param cellSize Розмір клітинки.
 * @param padding Відступ від країв.
 */
void draw_grid_gfx(int screenWidth, int screenHeight, int cellSize, int padding);

#endif // DRAW_GRID_GFX_H
