/**
============================================================================
bezier.c — Реалізація кривих Безьє
============================================================================
🔹 Параметрична кубічна форма (De Casteljau спрощений)
🔹 Фіксована точність: 32 сегменти (достатньо для UI)
*/
#include "bezier.h"
#include "gfx.h"
#include <math.h>

/* ============================================================================
🔹 КУБІЧНА КРИВА БЕЗЬЄ (EX)
============================================================================ */
void DrawBezierEx(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int segments, uint32_t color) {
    if (segments < 2) segments = 2;
    gfx_set_color_uint32(color);

    float prev_x = (float)x0;
    float prev_y = (float)y0;

    /* B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3 */
    for (int i = 1; i <= segments; i++) {
        float t = (float)i / (float)segments;
        float u = 1.0f - t;
        float u2 = u * u, t2 = t * t;
        float u3 = u2 * u, t3 = t2 * t;

        float cx = u3*x0 + 3*u2*t*x1 + 3*u*t2*x2 + t3*x3;
        float cy = u3*y0 + 3*u2*t*y1 + 3*u*t2*y2 + t3*y3;

        gfx_line((int)prev_x, (int)prev_y, (int)cx, (int)cy);
        prev_x = cx;
        prev_y = cy;
    }
}

/* ============================================================================
🔹 КУБІЧНА КРИВА БЕЗЬЄ (Default 32 segments)
============================================================================ */
void DrawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    DrawBezierEx(x0, y0, x1, y1, x2, y2, x3, y3, 32, color);
}
