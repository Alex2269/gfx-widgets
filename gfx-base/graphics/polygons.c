/**
============================================================================
polygons.c — Реалізація заповнених фігур для X11/gfx (скалярний API)
============================================================================
🔹 Всі функції використовують gfx_set_color_uint32 + gfx_line
🔹 Scanline-fill гарантує стабільність без артефактів
🔹 Максимальна кількість перетинів на рядок: 64
*/
#include "polygons.h"
#include "gfx.h"
#include <math.h>

/* ============================================================================
🔹 ЗАПОВНЕНИЙ БАГАТОКУТНИК (SCANLINE FILL)
============================================================================ */
void DrawPolygon(int *x, int *y, int pointCount, uint32_t color) {
    if (x == NULL || y == NULL || pointCount < 3) return;

    /* 1️⃣ Bounding box по Y (оптимізуємо цикл сканлайну) */
    int minY = y[0];
    int maxY = y[0];
    for (int i = 1; i < pointCount; i++) {
        if (y[i] < minY) minY = y[i];
        if (y[i] > maxY) maxY = y[i];
    }

    /* 2️⃣ Статичний буфер перетинів (без malloc) */
    #define MAX_INTERSECTIONS 64
    int xints[MAX_INTERSECTIONS];
    
    gfx_set_color_uint32(color);

    /* 3️⃣ Сканлайн-заповнення */
    for (int scanline = minY; scanline <= maxY; scanline++) {
        int n = 0; /* Кількість знайдених перетинів */

        for (int i = 0; i < pointCount; i++) {
            int j = (i + 1) % pointCount;
            
            float y1 = (float)y[i], y2 = (float)y[j];
            float x1 = (float)x[i], x2 = (float)x[j];

            /* Перетин ребра з горизонталлю scanline
               Умова (y1 <= scanline && y2 > scanline) || (y2 <= scanline && y1 > scanline)
               уникає подвійного підрахунку вершин */
            if ((y1 <= scanline && y2 > scanline) || (y2 <= scanline && y1 > scanline)) {
                if (n < MAX_INTERSECTIONS) {
                    xints[n++] = (int)(x1 + (scanline - y1) * (x2 - x1) / (y2 - y1));
                }
            }
        }

        /* 4️⃣ Сортування перетинів зліва направо */
        for (int a = 0; a < n - 1; a++) {
            for (int b = a + 1; b < n; b++) {
                if (xints[a] > xints[b]) {
                    int tmp = xints[a]; xints[a] = xints[b]; xints[b] = tmp;
                }
            }
        }

        /* 5️⃣ Заповнення парами перетинів */
        for (int k = 0; k < n; k += 2) {
            if (k + 1 < n) {
                gfx_line(xints[k], scanline, xints[k + 1], scanline);
            }
        }
    }
    #undef MAX_INTERSECTIONS
}
