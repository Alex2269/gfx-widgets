/**
============================================================================
lines.c — Реалізація примітивів для малювання ліній
============================================================================
🔹 Всі функції використовують gfx_set_color_uint32 + gfx_line/DrawPixel
🔹 Публічний API: тільки int x, y. Жодних Vector2!
*/
#include "lines.h"
#include "gfx.h"       // Для gfx_line, gfx_set_color_uint32, DrawPixel
#include <math.h>
#include <stdlib.h>

/* ============================================================================
🔹 БАЗОВА ЛІНІЯ (1px)
============================================================================ */
void DrawLine(int x1, int y1, int x2, int y2, uint32_t color) {
    gfx_set_color_uint32(color);
    gfx_line(x1, y1, x2, y2);
}

/* ============================================================================
🔹 ТОНКА ЛІНІЯ З ТОВЩИНОЮ (Bresenham + перпендикуляр)
============================================================================ */
void DrawThinLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    float len = sqrtf((float)dx*dx + (float)dy*dy);
    if (len < 1.0f) len = 1.0f;
    
    float ux = dx / len;
    float uy = dy / len;
    float px = -uy;  // Перпендикуляр
    float py =  ux;

    for (int i = -(thickness/2); i <= thickness/2; i++) {
        int ox = (int)(px * i);
        int oy = (int)(py * i);
        
        /* Алгоритм Брезенхема для кожної паралельної лінії */
        int x = x1 + ox, y = y1 + oy;
        int err = 0, derr = abs(dy), dlen = abs(dx);
        int stepx = (dx > 0) ? 1 : -1;
        int stepy = (dy > 0) ? 1 : -1;
        
        if (dlen > derr) {
            for (int t = 0; t <= dlen; t++) {
                DrawPixel(x, y, color);
                err += derr;
                if (err >= dlen) { err -= dlen; y += stepy; }
                x += stepx;
            }
        } else {
            for (int t = 0; t <= derr; t++) {
                DrawPixel(x, y, color);
                err += dlen;
                if (err >= derr) { err -= derr; x += stepx; }
                y += stepy;
            }
        }
    }
}

/* ============================================================================
🔹 ТОВСТА ЛІНІЯ (квадратики вздовж траєкторії)
============================================================================ */
void DrawThickLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    float len = sqrtf((float)dx*dx + (float)dy*dy);
    if (len < 1.0f) len = 1.0f;
    
    float ux = dx / len;
    float uy = dy / len;
    float px = -uy, py = ux;
    float ox = px * (thickness/2.0f);
    float oy = py * (thickness/2.0f);

    for (int i = -(thickness/2); i <= thickness/2; i++) {
        int offx = (int)(ox * i);
        int offy = (int)(oy * i);
        
        int x = x1 + offx, y = y1 + offy;
        int err = 0, derr = abs(dy), dlen = abs(dx);
        int stepx = (dx > 0) ? 1 : -1;
        int stepy = (dy > 0) ? 1 : -1;
        
        if (dlen > derr) {
            for (int t = 0; t <= dlen; t++) {
                DrawRectangle(x, y, thickness, thickness, color);
                err += derr;
                if (err >= dlen) { err -= dlen; y += stepy; }
                x += stepx;
            }
        } else {
            for (int t = 0; t <= derr; t++) {
                DrawRectangle(x, y, thickness, thickness, color);
                err += dlen;
                if (err >= derr) { err -= derr; x += stepx; }
                y += stepy;
            }
        }
    }
}

/* ============================================================================
🔹 ЛІНІЯ З ТОВЩИНОЮ (скалярна версія для сумісності)
============================================================================ */
void DrawLineEx(int x1, int y1, int x2, int y2, float thickness, uint32_t color) {
    DrawThickLine(x1, y1, x2, y2, (int)thickness, color);
}

/* ============================================================================
🔹 ЛІНІЯ З ТОВЩИНОЮ (Vector2 версія для сумісності)                           *
 ============================================================================ */
void DrawLineExVec2(Vector2 p1, Vector2 p2, float thickness, uint32_t color) {
    gfx_set_color_uint32(color);

    if (thickness <= 1.0f) {
        gfx_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
        return;
    }

    /* Малюємо кілька паралельних ліній для імітації товщини */
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.001f) return;

    float nx = -dy / len;  /* Нормаль */
    float ny = dx / len;

    int layers = (int)thickness;
    for (int i = -layers/2; i <= layers/2; i++) {
        float ox = nx * i;
        float oy = ny * i;
        gfx_line((int)(p1.x + ox), (int)(p1.y + oy),
                 (int)(p2.x + ox), (int)(p2.y + oy));
    }
}
