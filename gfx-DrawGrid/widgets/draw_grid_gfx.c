/* draw_grid_gfx.c */
#include "draw_grid_gfx.h"

/* ================= РЕАЛІЗАЦІЯ ================= */

void draw_grid_layer_gfx(int startX, int endX, int stepX,
                         int startY, int endY, int stepY,
                         uint32_t color) {
    // Малюємо точки сітки як маленькі прямокутники 2x2
    for (int y = startY; y <= endY; y += stepY) {
        for (int x = startX; x <= endX; x += stepX) {
            DrawRectangleFast(x, y, 2, 2, color);
        }
    }
}

void draw_grid_gfx(int screenWidth, int screenHeight, int cellSize, int padding) {
    // Визначаємо область для малювання сітки з урахуванням padding
    int startX = padding;
    int startY = padding;
    int endX = screenWidth - padding;
    int endY = screenHeight - padding;
    int width = endX - startX;
    int height = endY - startY;

    // Визначаємо скільки клітинок поміщається по горизонталі і вертикалі
    int cellsX = width / cellSize;
    int cellsY = height / cellSize;

    // Вираховуємо відступи, щоб сітка була центрована всередині області
    int offsetX = startX + (width - cellsX * cellSize) / 2;
    int offsetY = startY + (height - cellsY * cellSize) / 2;

    // Крок між точками для імітації ліній (1/5 від розміру клітинки)
    int dotSpacing = cellSize / 5;

    // Горизонтальні лінії (сині точки)
    draw_grid_layer_gfx(offsetX, offsetX + cellsX * cellSize, dotSpacing,
                        offsetY, offsetY + cellsY * cellSize, cellSize,
                        0x0000FF); // BLUE

    // Вертикальні лінії (блакитні точки)
    draw_grid_layer_gfx(offsetX, offsetX + cellsX * cellSize, cellSize,
                        offsetY, offsetY + cellsY * cellSize, dotSpacing,
                        0x87CEEB); // SKYBLUE
}
