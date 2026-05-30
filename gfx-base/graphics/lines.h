/**
============================================================================
lines.h — Примітиви для малювання ліній (скалярний API)
============================================================================
🔹 Жодних Vector2 у публічних функціях! Тільки int x, y
🔹 Підтримує: тонкі/товсті лінії, товщину, кольори (0xRRGGBB)
@version 1.0 (Scalar-Only)
@license MIT
*/
#ifndef LINES_H
#define LINES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================= БАЗОВІ ЛІНІЇ ================= */
/** 🔹 Проста лінія (1px, оптимізована через gfx_line) */
void DrawLine(int x1, int y1, int x2, int y2, uint32_t color);

/** 🔹 Тонка лінія з товщиною (алгоритм Брезенхема + перпендикуляр) */
void DrawThinLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color);

/** 🔹 Товста лінія (малює квадратики вздовж траєкторії) */
void DrawThickLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color);

/** 🔹 Лінія з товщиною (Vector2-версія для сумісності) */
void DrawLineEx(int x1, int y1, int x2, int y2, float thickness, uint32_t color);

#ifdef __cplusplus
}
#endif
#endif // LINES_H
