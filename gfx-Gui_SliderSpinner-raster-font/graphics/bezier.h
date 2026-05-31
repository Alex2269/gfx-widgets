/**
============================================================================
bezier.h — Криві Безьє (скалярний API)
============================================================================
🔹 Жодних Vector2! Тільки int координати
🔹 Кубічна крива Безьє (P0, P1, P2, P3) з фіксованою дискретизацією
@version 1.0 (Scalar-Only)
@license MIT
*/
#ifndef BEZIER_H
#define BEZIER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 🔹 Кубічна крива Безьє (32 сегменти за замовчуванням) */
void DrawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

/** 🔹 Кубічна крива Безьє з налаштовуваною кількістю сегментів */
void DrawBezierEx(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int segments, uint32_t color);

#ifdef __cplusplus
}
#endif
#endif // BEZIER_H
