/**
============================================================================
circles.c — Реалізація примітивів для кіл (скалярний API)
============================================================================
🔹 Всі функції використовують gfx_set_color_uint32 + gfx_line/DrawPixel
🔹 Публічний API: тільки int координати
🔹 Алгоритми оптимізовані для швидкості на X11
*/
#include "circles.h"
#include "gfx.h"       // Для gfx_line, gfx_set_color_uint32, DrawPixel
#include <math.h>
#include <stdlib.h>

/* ============================================================================
🔹 ЗАПОВНЕНЕ КОЛО (SCANLINE FILL)
============================================================================ */
void DrawCircleFilled(int cx, int cy, int radius, uint32_t color) {
    if (radius <= 0) return;
    
    gfx_set_color_uint32(color);
    
    /* Midpoint Circle Algorithm для знаходження меж кола на кожному рядку */
    int x = radius;
    int y = 0;
    int err = 0;
    
    /* Масив для зберігання x-меж для кожного scanline (відносно центру) */
    int *x_left = (int*)calloc(radius + 1, sizeof(int));
    int *x_right = (int*)calloc(radius + 1, sizeof(int));
    if (!x_left || !x_right) return; /* Memory fail-safe */
    
    /* Ініціалізація */
    for (int i = 0; i <= radius; i++) {
        x_left[i] = -radius;
        x_right[i] = radius;
    }
    
    while (x >= y) {
        /* Оновлюємо межі для 8 октантів (симетрія) */
        if (y <= radius) {
            if (-x < x_left[y]) x_left[y] = -x;
            if ( x > x_right[y]) x_right[y] =  x;
            if (-y < x_left[x]) x_left[x] = -y;
            if ( y > x_right[x]) x_right[x] =  y;
        }
        if (x <= radius) {
            if (-y < x_left[x]) x_left[x] = -y;
            if ( y > x_right[x]) x_right[x] =  y;
            if (-x < x_left[y]) x_left[y] = -x;
            if ( x > x_right[y]) x_right[y] =  x;
        }
        
        y++;
        err += 1 + 2*y;
        if (2*(err - x) + 1 > 0) {
            x--;
            err += 1 - 2*x;
        }
    }
    
    /* Малюємо горизонтальні лінії для кожної половини */
    for (int dy = 0; dy <= radius; dy++) {
        /* Верхня половина */
        if (x_left[dy] <= x_right[dy]) {
            gfx_line(cx + x_left[dy], cy + dy, cx + x_right[dy], cy + dy);
        }
        /* Нижня половина (симетрія) */
        if (dy > 0 && x_left[dy] <= x_right[dy]) {
            gfx_line(cx + x_left[dy], cy - dy, cx + x_right[dy], cy - dy);
        }
    }
    
    free(x_left);
    free(x_right);
}

/* ============================================================================
🔹 КОНТУР КОЛА (1px) — Midpoint Circle Algorithm
============================================================================ */
void DrawCircleLines(int cx, int cy, int radius, uint32_t color) {
    if (radius <= 0) return;
    
    gfx_set_color_uint32(color);
    
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        /* 8-точкова симетрія */
        DrawPixel(cx + x, cy + y, color);
        DrawPixel(cx + y, cy + x, color);
        DrawPixel(cx - y, cy + x, color);
        DrawPixel(cx - x, cy + y, color);
        DrawPixel(cx - x, cy - y, color);
        DrawPixel(cx - y, cy - x, color);
        DrawPixel(cx + y, cy - x, color);
        DrawPixel(cx + x, cy - y, color);
        
        y++;
        err += 1 + 2*y;
        if (2*(err - x) + 1 > 0) {
            x--;
            err += 1 - 2*x;
        }
    }
}

/* ============================================================================
🔹 КОНТУР КОЛА ЗІ ЗМІННОЮ ТОВЩИНОЮ
============================================================================ */
void DrawCircleLinesEx(int cx, int cy, int radius, float thickness, uint32_t color) {
    if (radius <= 0 || thickness <= 0.0f) return;
    
    int t = (int)thickness;
    if (t < 1) t = 1;
    
    /* Малюємо кілька концентричних кіл для імітації товщини */
    for (int r = radius; r > radius - t && r > 0; r--) {
        DrawCircleLines(cx, cy, r, color);
    }
}

/* ============================================================================
🔹 УНІВЕРСАЛЬНЕ КОЛО (fill = true → заповнене, false → контур)
============================================================================ */
void DrawCircle(int cx, int cy, int radius, uint32_t color, bool fill) {
    if (fill) {
        DrawCircleFilled(cx, cy, radius, color);
    } else {
        DrawCircleLines(cx, cy, radius, color);
    }
}
