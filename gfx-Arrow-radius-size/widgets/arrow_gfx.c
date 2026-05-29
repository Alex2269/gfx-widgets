/* arrow_gfx.c */
#include "arrow_gfx.h"
#include <stdio.h>
#include <stdlib.h>

/* Внутрішня функція сортування вершин за Y для Scanline */
static void sort_vertices(int *x, int *y, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (y[j] > y[j + 1]) {
                int tmp = y[j]; y[j] = y[j + 1]; y[j + 1] = tmp;
                tmp = x[j]; x[j] = x[j + 1]; x[j + 1] = tmp;
            }
        }
    }
}

/**
 * 🔹 Scanline Fill: Малює трикутник горизонтальними лініями.
 * Це набагато швидше, ніж малювати піксель за пікселем.
 */
void gfx_draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    int x[3] = {x0, x1, x2};
    int y[3] = {y0, y1, y2};
    
    /* Сортуємо вершини: v0.y <= v1.y <= v2.y */
    sort_vertices(x, y, 3);
    
    x0 = x[0]; y0 = y[0];
    x1 = x[1]; y1 = y[1];
    x2 = x[2]; y2 = y[2];

    if (y0 == y2) return; /* Вироджений трикутник */

    gfx_set_color_uint32(color);

    /* Верхня половина (v0 -> v1) */
    if (y1 > y0) {
        for (int y = y0; y <= y1; y++) {
            float dy_top = (float)(y - y0) / (y1 - y0);
            float dy_bot = (float)(y - y0) / (y2 - y0);
            int xa = (int)(x0 + (x1 - x0) * dy_top);
            int xb = (int)(x0 + (x2 - x0) * dy_bot);
            if (xa > xb) { int t = xa; xa = xb; xb = t; }
            gfx_line(xa, y, xb, y);
        }
    }

    /* Нижня половина (v1 -> v2) */
    if (y2 > y1) {
        for (int y = y1; y <= y2; y++) {
            float dy_top = (float)(y - y1) / (y2 - y1);
            float dy_bot = (float)(y - y0) / (y2 - y0);
            int xa = (int)(x1 + (x2 - x1) * dy_top);
            int xb = (int)(x0 + (x2 - x0) * dy_bot);
            if (xa > xb) { int t = xa; xa = xb; xb = t; }
            gfx_line(xa, y, xb, y);
        }
    }
}

/**
 * 🔹 Контур рівностороннього трикутника
 */
void gfx_draw_triangle_lines_rotated(float cx, float cy, float size, float rotation_deg, uint32_t color) {
    float rad = rotation_deg * DEG2RAD;
    float angles[3] = { rad, rad + 2.0f * M_PI / 3.0f, rad + 4.0f * M_PI / 3.0f };
    int vx[3], vy[3];

    for (int i = 0; i < 3; i++) {
        vx[i] = (int)(cx + size * cosf(angles[i]));
        vy[i] = (int)(cy + size * sinf(angles[i]));
    }

    gfx_set_color_uint32(color);
    gfx_line(vx[0], vy[0], vx[1], vy[1]);
    gfx_line(vx[1], vy[1], vx[2], vy[2]);
    gfx_line(vx[2], vy[2], vx[0], vy[0]);
}

/**
 * 🔹 Заповнений рівносторонній трикутник
 */
void gfx_draw_filled_triangle_rotated(float cx, float cy, float size, float rotation_deg, uint32_t color) {
    float rad = rotation_deg * DEG2RAD;
    float angles[3] = { rad, rad + 2.0f * M_PI / 3.0f, rad + 4.0f * M_PI / 3.0f };
    int vx[3], vy[3];

    for (int i = 0; i < 3; i++) {
        vx[i] = (int)(cx + size * cosf(angles[i]));
        vy[i] = (int)(cy + size * sinf(angles[i]));
    }

    gfx_draw_filled_triangle(vx[0], vy[0], vx[1], vy[1], vx[2], vy[2], color);
}

/**
 * 🔹 Стрілка (Arrowhead)
 * Базова точка (bx, by) є "хвостом" стрілки.
 * Стрілка дивиться в напрямку angle_rad.
 */
void gfx_draw_arrow(float bx, float by, float angle_rad, float size, uint32_t color) {
    /* Вершини: P0 (хвіст), P1 (праве крило), P2 (ліве крило) */
    int x0 = (int)bx;
    int y0 = (int)by;
    int x1 = (int)(bx + size * cosf(angle_rad + DEG2RAD * 30.0f));
    int y1 = (int)(by + size * sinf(angle_rad + DEG2RAD * 30.0f));
    int x2 = (int)(bx + size * cosf(angle_rad - DEG2RAD * 30.0f));
    int y2 = (int)(by + size * sinf(angle_rad - DEG2RAD * 30.0f));

    gfx_draw_filled_triangle(x0, y0, x1, y1, x2, y2, color);
}
