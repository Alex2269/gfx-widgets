/**
============================================================================
polygons.h — Примітиви заповнених фігур (чистий скалярний API)
============================================================================
🔹 Жодних Vector2! Тільки int x, y та масиви координат
🔹 Scanline-fill гарантує стабільність без артефактів на X11
@version 1.0 (Scalar-Only)
@license MIT
*/
#ifndef POLYGONS_H
#define POLYGONS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================= БАГАТОКУТНИК ================= */
/** 🔹 Заповнений багатокутник (scanline fill)
 * @param x Масив X-координат вершин
 * @param y Масив Y-координат вершин
 * @param pointCount Кількість вершин (має бути >= 3)
 * @param color Колір заповнення
 */
void DrawPolygon(int *x, int *y, int pointCount, uint32_t color);

#ifdef __cplusplus
}
#endif
#endif // POLYGONS_H
