/**
============================================================================
triangles.c — Реалізація трикутників (чисто скалярні координати)
============================================================================
🔹 Scanline-fill використовує gfx_line (дуже швидко на X11)
🔹 Публічний API: тільки int x, y. Жодних Vector2!
*/
#include "triangles.h"
#include "graphics.h"  // Для DrawLine, DrawLineEx
#include "gfx.h"       // Для gfx_set_color_uint32, gfx_line
#include <math.h>

/* ============================================================================
🔹 ЗАПОВНЕНИЙ ТРИКУТНИК (SCANLINE)
============================================================================ */
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    /* 1️⃣ Сортування вершин за Y (y1 <= y2 <= y3) */
    if (y1 > y2) { int t=y1; y1=y2; y2=t; t=x1; x1=x2; x2=t; }
    if (y2 > y3) { int t=y2; y2=y3; y3=t; t=x2; x2=x3; x3=t; }
    if (y1 > y2) { int t=y1; y1=y2; y2=t; t=x1; x1=x2; x2=t; }
    if (y1 == y3) return; /* Вироджений трикутник */

    gfx_set_color_uint32(color);

    /* 2️⃣ Обернені нахили (dx/dy) */
    float invslope12 = (float)(x2 - x1) / (y2 - y1 + 1);
    float invslope13 = (float)(x3 - x1) / (y3 - y1 + 1);
    float invslope23 = (float)(x3 - x2) / (y3 - y2 + 1);

    float curx1 = (float)x1;
    float curx2 = (float)x1;

    /* 3️⃣ Верхня половина (від y1 до y2) */
    for (int scanline = y1; scanline <= y2; scanline++) {
        int xa = (int)curx1;
        int xb = (int)curx2;
        if (xa > xb) { int t=xa; xa=xb; xb=t; }
        gfx_line(xa, scanline, xb, scanline);
        curx1 += invslope12;
        curx2 += invslope13;
    }

    /* 4️⃣ Нижня половина (від y2 до y3) */
    curx1 = (float)x2;
    curx2 = (float)x1 + invslope13 * (y2 - y1);
    for (int scanline = y2; scanline <= y3; scanline++) {
        int xa = (int)curx1;
        int xb = (int)curx2;
        if (xa > xb) { int t=xa; xa=xb; xb=t; }
        gfx_line(xa, scanline, xb, scanline);
        curx1 += invslope23;
        curx2 += invslope13;
    }
}

/* ============================================================================
🔹 КОНТУР (1px)
============================================================================ */
void DrawTriangleLines(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    DrawLine(x1, y1, x2, y2, color);
    DrawLine(x2, y2, x3, y3, color);
    DrawLine(x3, y3, x1, y1, color);
}

/* ============================================================================
🔹 КОНТУР ЗІ ЗМІННОЮ ТОВЩИНОЮ
============================================================================ */
void DrawTriangleLinesEx(int x1, int y1, int x2, int y2, int x3, int y3, float thickness, uint32_t color) {
    /* Внутрішньо кастимо до Vector2 лише для сумісності з існуючим DrawLineEx.
       Публічний API залишається 100% вектор-фрі! */
    DrawLineEx(x1, y1, x2, y2, thickness, color);
    DrawLineEx(x2, y2, x3, y3, thickness, color);
    DrawLineEx(x3, y3, x1, y1, thickness, color);
}
