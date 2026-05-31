/**
============================================================================
circles.h — Примітиви для малювання кіл (скалярний API)
============================================================================
🔹 Жодних Vector2! Тільки int координати
🔹 Підтримує: заповнені кола, контурні кола, товщину контуру
🔹 Алгоритми: Midpoint Circle + Scanline Fill (оптимізовано під gfx)
@version 1.0 (Scalar-Only)
@license MIT
*/
#ifndef CIRCLES_H
#define CIRCLES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================= ЗАПОВНЕНІ КОЛА ================= */
/** 🔹 Заповнене коло (scanline-алгоритм, O(radius) ліній) */
void DrawCircleFilled(int cx, int cy, int radius, uint32_t color);

/* ================= КОНТУРНІ КОЛА ================= */
/** 🔹 Контур кола (1px, Midpoint Circle Algorithm) */
void DrawCircleLines(int cx, int cy, int radius, uint32_t color);

/** 🔹 Контур кола зі змінною товщиною (малює кілька концентричних кіл) */
void DrawCircleLinesEx(int cx, int cy, int radius, float thickness, uint32_t color);

/* ================= УНІВЕРСАЛЬНІ ================= */
/** 🔹 Коло (заповнене або контурне, залежно від fill) */
void DrawCircle(int cx, int cy, int radius, uint32_t color, bool fill);

#ifdef __cplusplus
}
#endif
#endif // CIRCLES_H
