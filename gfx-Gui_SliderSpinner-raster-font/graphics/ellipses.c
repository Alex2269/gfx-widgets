/**
============================================================================
ellipses.c — Реалізація еліпсів та дуг (скалярний API)
============================================================================
🔹 Всі функції використовують gfx_set_color_uint32 + gfx_line
🔹 Публічний API: тільки int координати
🔹 Алгоритми оптимізовані для швидкості на X11
*/
#include "ellipses.h"
#include "gfx.h"       // Для gfx_line, gfx_set_color_uint32
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef DEG2RAD
#define DEG2RAD (M_PI / 180.0f)
#endif

/* ============================================================================
🔹 ЗАПОВНЕНИЙ ЕЛІПС (SCANLINE FILL)
============================================================================ */
void DrawEllipseFilled(int cx, int cy, int rx, int ry, uint32_t color) {
    if (rx <= 0 || ry <= 0) return;
    
    gfx_set_color_uint32(color);
    
    /* Midpoint Ellipse Algorithm для знаходження меж на кожному рядку */
    int x = 0, y = ry;
    long long a2 = (long long)rx * rx;
    long long b2 = (long long)ry * ry;
    long long a2b2 = a2 * b2;
    
    /* Буфер для x-меж кожного scanline (відносно центру) */
    int *x_left = (int*)calloc(ry + 1, sizeof(int));
    int *x_right = (int*)calloc(ry + 1, sizeof(int));
    if (!x_left || !x_right) return;
    
    for (int i = 0; i <= ry; i++) {
        x_left[i] = -rx;
        x_right[i] = rx;
    }
    
    /* Регіон 1: dy/dx > -1 */
    long long p1 = b2 - a2 * ry + a2 / 4;
    while (b2 * (x + 1) < a2 * (y - 0.5f)) {
        if (y <= ry) {
            if (-x < x_left[y]) x_left[y] = -x;
            if ( x > x_right[y]) x_right[y] =  x;
        }
        if (p1 < 0) {
            p1 += b2 * (2 * x + 3);
        } else {
            if (y > 0 && y <= ry) {
                if (-x < x_left[y]) x_left[y] = -x;
                if ( x > x_right[y]) x_right[y] =  x;
            }
            p1 += b2 * (2 * x + 3) - a2 * (2 * y - 2);
            y--;
        }
        x++;
    }
    
    /* Регіон 2: dy/dx <= -1 */
    long long p2 = b2 * (x + 0.5f) * (x + 0.5f) + a2 * (y - 1) * (y - 1) - a2b2;
    while (y >= 0) {
        if (y >= 0 && y <= ry) {
            if (-x < x_left[y]) x_left[y] = -x;
            if ( x > x_right[y]) x_right[y] =  x;
        }
        if (p2 > 0) {
            p2 += a2 * (3 - 2 * y);
        } else {
            x++;
            p2 += b2 * (2 * x + 2) + a2 * (3 - 2 * y);
        }
        y--;
    }
    
    /* Малюємо горизонтальні лінії */
    for (int dy = -ry; dy <= ry; dy++) {
        int idx = (dy < 0) ? -dy : dy;
        if (x_left[idx] <= x_right[idx]) {
            gfx_line(cx + x_left[idx], cy + dy, cx + x_right[idx], cy + dy);
        }
    }
    
    free(x_left);
    free(x_right);
}

/* ============================================================================
🔹 КОНТУР ЕЛІПСА (1px) — Midpoint Ellipse Algorithm
============================================================================ */
void DrawEllipseLines(int cx, int cy, int rx, int ry, uint32_t color) {
    if (rx <= 0 || ry <= 0) return;
    
    gfx_set_color_uint32(color);
    
    int x = 0, y = ry;
    long long a2 = (long long)rx * rx;
    long long b2 = (long long)ry * ry;
    long long a2b2 = a2 * b2;
    
    /* Регіон 1 */
    long long p1 = b2 - a2 * ry + a2 / 4;
    while (b2 * (x + 1) < a2 * (y - 0.5f)) {
        DrawPixel(cx + x, cy + y, color);
        DrawPixel(cx - x, cy + y, color);
        DrawPixel(cx + x, cy - y, color);
        DrawPixel(cx - x, cy - y, color);
        
        if (p1 < 0) {
            p1 += b2 * (2 * x + 3);
        } else {
            DrawPixel(cx + x, cy + y - 1, color);
            DrawPixel(cx - x, cy + y - 1, color);
            DrawPixel(cx + x, cy - y + 1, color);
            DrawPixel(cx - x, cy - y + 1, color);
            p1 += b2 * (2 * x + 3) - a2 * (2 * y - 2);
            y--;
        }
        x++;
    }
    
    /* Регіон 2 */
    long long p2 = b2 * (x + 0.5f) * (x + 0.5f) + a2 * (y - 1) * (y - 1) - a2b2;
    while (y >= 0) {
        DrawPixel(cx + x, cy + y, color);
        DrawPixel(cx - x, cy + y, color);
        DrawPixel(cx + x, cy - y, color);
        DrawPixel(cx - x, cy - y, color);
        
        if (p2 > 0) {
            p2 += a2 * (3 - 2 * y);
        } else {
            x++;
            p2 += b2 * (2 * x + 2) + a2 * (3 - 2 * y);
        }
        y--;
    }
}

/* ============================================================================
🔹 КОНТУР ЕЛІПСА ЗІ ЗМІННОЮ ТОВЩИНОЮ
============================================================================ */
void DrawEllipseLinesEx(int cx, int cy, int rx, int ry, float thickness, uint32_t color) {
    if (thickness <= 0.0f) return;
    
    int t = (int)thickness;
    if (t < 1) t = 1;
    
    /* Малюємо кілька концентричних еліпсів для імітації товщини */
    for (int r = 0; r < t; r++) {
        int irx = rx - r, iry = ry - r;
        if (irx > 0 && iry > 0) {
            DrawEllipseLines(cx, cy, irx, iry, color);
        }
    }
}

/* ============================================================================
🔹 ДОПОМІЖНА: перевірка, чи кут у діапазоні [start, end]
============================================================================ */
static inline bool _AngleInRange(float angle, float start, float end) {
    /* Нормалізуємо кути до [0..360) */
    while (angle < 0) angle += 360.0f;
    while (start < 0) start += 360.0f;
    while (end < 0) end += 360.0f;
    while (angle >= 360) angle -= 360.0f;
    while (start >= 360) start -= 360.0f;
    while (end >= 360) end -= 360.0f;
    
    if (start <= end) {
        return (angle >= start && angle <= end);
    } else {
        /* Перехід через 360° */
        return (angle >= start || angle <= end);
    }
}

/* ============================================================================
🔹 ДУГА ЕЛІПСА (КОНТУР)
============================================================================ */
void DrawArc(int cx, int cy, int radius, float startAngle, float endAngle, uint32_t color) {
    if (radius <= 0) return;
    
    gfx_set_color_uint32(color);
    
    /* Параметрична форма еліпса (для дуги використовуємо коло) */
    float step = 1.0f / radius;  /* Крок у радіанах для плавності */
    float prevX = -1, prevY = -1;
    
    for (float angle = startAngle; 
         (endAngle >= startAngle) ? (angle <= endAngle) : (angle >= endAngle);
         angle += (endAngle >= startAngle) ? step : -step) {
        
        float rad = angle * DEG2RAD;
        int x = cx + (int)(radius * cosf(rad));
        int y = cy + (int)(radius * sinf(rad));
        
        if (prevX >= 0) {
            gfx_line((int)prevX, (int)prevY, x, y);
        }
        prevX = x; prevY = y;
    }
}

/* ============================================================================
🔹 ЗАПОВНЕНА ДУГА / СЕКТОР (PIE SLICE)
============================================================================ */
void DrawArcFilled(int cx, int cy, int rx, int ry, float startAngle, float endAngle, uint32_t color) {
    if (rx <= 0 || ry <= 0) return;
    
    gfx_set_color_uint32(color);
    
    /* Scanline fill з перевіркою кута */
    for (int dy = -ry; dy <= ry; dy++) {
        /* Знаходимо x-межі еліпса для цього dy */
        float y_norm = (float)dy / ry;
        if (y_norm * y_norm > 1.0f) continue;
        
        int xmax = (int)(rx * sqrtf(1.0f - y_norm * y_norm));
        
        for (int dx = -xmax; dx <= xmax; dx++) {
            /* Перевіряємо, чи точка у секторі */
            float angle = atan2f((float)dy, (float)dx) / DEG2RAD;
            if (angle < 0) angle += 360.0f;
            
            if (_AngleInRange(angle, startAngle, endAngle)) {
                DrawPixel(cx + dx, cy + dy, color);
            }
        }
    }
}

/* ============================================================================
🔹 УНІВЕРСАЛЬНА ДУГА (fill = true → заповнена, false → контур)
============================================================================ */
void DrawArcEx(int cx, int cy, int rx, int ry, float startAngle, float endAngle,
               float thickness, uint32_t color, bool fill) {
    if (fill) {
        DrawArcFilled(cx, cy, rx, ry, startAngle, endAngle, color);
    } else {
        /* Малюємо контур з товщиною через кілька концентричних дуг */
        int t = (int)thickness;
        if (t < 1) t = 1;
        
        for (int r = 0; r < t; r++) {
            int irx = rx - r, iry = ry - r;
            if (irx > 0 && iry > 0) {
                DrawArc(cx, cy, irx, startAngle, endAngle, color);
            }
        }
    }
}
