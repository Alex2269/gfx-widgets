/* arrow_gfx.c */
#include "arrow_gfx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

/* ================= SCANLINE ЗАПОВНЕННЯ ================= */
/* Використовує gfx_line для горизонтальних рядків, що дуже швидко на X11 */
void gfx_draw_filled_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    /* Сортуємо вершини за Y (y1 <= y2 <= y3) */
    if (y1 > y2) { int t = y1; y1 = y2; y2 = t; int tx = x1; x1 = x2; x2 = tx; }
    if (y2 > y3) { int t = y2; y2 = y3; y3 = t; int tx = x2; x2 = x3; x3 = tx; }
    if (y1 > y2) { int t = y1; y1 = y2; y2 = t; int tx = x1; x1 = x2; x2 = tx; }
    
    if (y1 == y3) return; /* Вироджений трикутник */

    gfx_set_color_uint32(color);

    /* Верхня половина */
    for (int y = y1; y <= y2; y++) {
        float dy_top = (y2 - y1) > 0 ? (float)(y - y1) / (y2 - y1) : 0.0f;
        float dy_bot = (y3 - y1) > 0 ? (float)(y - y1) / (y3 - y1) : 0.0f;
        int xa = (int)(x1 + (x2 - x1) * dy_top);
        int xb = (int)(x1 + (x3 - x1) * dy_bot);
        if (xa > xb) { int t = xa; xa = xb; xb = t; }
        gfx_line(xa, y, xb, y);
    }

    /* Нижня половина */
    for (int y = y2; y <= y3; y++) {
        float dy_top = (y3 - y2) > 0 ? (float)(y - y2) / (y3 - y2) : 0.0f;
        float dy_bot = (y3 - y1) > 0 ? (float)(y - y1) / (y3 - y1) : 0.0f;
        int xa = (int)(x2 + (x3 - x2) * dy_top);
        int xb = (int)(x1 + (x3 - x1) * dy_bot);
        if (xa > xb) { int t = xa; xa = xb; xb = t; }
        gfx_line(xa, y, xb, y);
    }
}

void gfx_draw_triangle_outline(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    gfx_set_color_uint32(color);
    gfx_line(x1, y1, x2, y2);
    gfx_line(x2, y2, x3, y3);
    gfx_line(x3, y3, x1, y1);
}

/* ================= НОВА ФУНКЦІЯ: ОБЕРТАЮЧИЙСЯ КОНТУР ================= */
void gfx_draw_triangle_outline_rotated(float cx, float cy, float size, float rotation_deg, uint32_t color) {
    float rad = rotation_deg * (PI / 180.0f);
    float h = size * sqrtf(3.0f) / 2.0f; /* Висота рівностороннього трикутника */
    
    /* Вершини відносно центру (0,0) */
    float px[3] = { 0.0f, -size/2.0f, size/2.0f };
    float py[3] = { -2.0f/3.0f * h, 1.0f/3.0f * h, 1.0f/3.0f * h };

    /* Обертання та зміщення до (cx, cy) */
    float cr = cosf(rad), sr = sinf(rad);
    int vx[3], vy[3];
    for (int i = 0; i < 3; i++) {
        vx[i] = (int)(cx + px[i]*cr - py[i]*sr);
        vy[i] = (int)(cy + px[i]*sr + py[i]*cr);
    }

    /* Малюємо контур */
    gfx_draw_triangle_outline(vx[0], vy[0], vx[1], vy[1], vx[2], vy[2], color);
}

/* ================= СТРІЛКА ТА ПОВЕРНУТИЙ ТРИКУТНИК ================= */

void gfx_draw_arrow(float cx, float cy, float angle_deg, float size, uint32_t color) {
    float rad = angle_deg * (PI / 180.0f);
    /* Кут розкриття стрілки: ±30° (PI/6) */
    float p0x = cx;
    float p0y = cy;
    float p1x = cx + size * cosf(rad + PI/6.0f);
    float p1y = cy + size * sinf(rad + PI/6.0f);
    float p2x = cx + size * cosf(rad - PI/6.0f);
    float p2y = cy + size * sinf(rad - PI/6.0f);

    gfx_draw_filled_triangle((int)p0x, (int)p0y, (int)p1x, (int)p1y, (int)p2x, (int)p2y, color);
}

void gfx_draw_rotated_triangle(float cx, float cy, float size, float rotation_deg, uint32_t color) {
    float rad = rotation_deg * (PI / 180.0f);
    float h = size * sqrtf(3.0f) / 2.0f; /* Висота рівностороннього трикутника */
    
    /* Вершини відносно центру (0,0) */
    float px[3] = { 0.0f, -size/2.0f, size/2.0f };
    float py[3] = { -2.0f/3.0f * h, 1.0f/3.0f * h, 1.0f/3.0f * h };

    /* Обертання та зміщення до (cx, cy) */
    float cr = cosf(rad), sr = sinf(rad);
    int vx[3], vy[3];
    for (int i = 0; i < 3; i++) {
        vx[i] = (int)(cx + px[i]*cr - py[i]*sr);
        vy[i] = (int)(cy + px[i]*sr + py[i]*cr);
    }

    gfx_draw_filled_triangle(vx[0], vy[0], vx[1], vy[1], vx[2], vy[2], color);
}
