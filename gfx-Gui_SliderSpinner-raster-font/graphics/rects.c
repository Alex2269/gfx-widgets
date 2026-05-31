/**
============================================================================
rects.c — Реалізація примітивів для прямокутників
============================================================================
🔹 Всі функції використовують gfx_set_color_uint32 + gfx_draw_fast_rect
🔹 Публічний API: тільки int координати
*/
#include "rects.h"
#include "gfx.h"       // Для gfx_draw_fast_rect, gfx_set_color_uint32, DrawPixel

/* ============================================================================
🔹 ЗАПОВНЕНИЙ ПРЯМОКУТНИК (попіксельний, для сумісності)
============================================================================ */
void DrawRectangle(int x, int y, int width, int height, uint32_t color) {
    if (width <= 0 || height <= 0) return;
    gfx_set_color_uint32(color);
    for (int py = y; py < y + height; py++) {
        for (int px = x; px < x + width; px++) {
            DrawPixel(px, py, color);
        }
    }
}

/* ============================================================================
🔹 КОНТУР ПРЯМОКУТНИКА (1px)
============================================================================ */
void DrawRectangleLines(int x, int y, int width, int height, uint32_t color) {
    if (width <= 0 || height <= 0) return;
    gfx_set_color_uint32(color);
    
    /* Верх і низ */
    for (int px = x; px < x + width; px++) {
        DrawPixel(px, y, color);
        DrawPixel(px, y + height - 1, color);
    }
    /* Ліво і право (без кутів, щоб не малювати двічі) */
    for (int py = y + 1; py < y + height - 1; py++) {
        DrawPixel(x, py, color);
        DrawPixel(x + width - 1, py, color);
    }
}

/* ============================================================================
🔹 ОПТИМІЗОВАНІ ВЕРСІЇ (gfx_draw_fast_rect)
============================================================================ */
void DrawRectangleFast(int x, int y, int width, int height, uint32_t color) {
    if (width < 0) { x += width; width = -width; }
    if (height < 0) { y += height; height = -height; }
    gfx_draw_fast_rect(x, y, width, height, color);
}

void DrawRectangleLinesFast(int x, int y, int width, int height, uint32_t color) {
    if (width < 0) { x += width; width = -width; }
    if (height < 0) { y += height; height = -height; }
    gfx_draw_fast_rect_outline(x, y, width, height, color);
}
