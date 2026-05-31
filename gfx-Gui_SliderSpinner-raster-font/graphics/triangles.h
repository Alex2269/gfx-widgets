/**
============================================================================
triangles.h — Примітиви трикутників (чистий скалярний API)
============================================================================
🔹 Жодних Vector2 у публічних функціях! Тільки int x, y
🔹 Підтримує: заповнення (scanline), контур, контур зі змінною товщиною
@version 1.0 (Scalar-Only)
@license MIT
*/
#ifndef TRIANGLES_H
#define TRIANGLES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================= ЗАПОВНЕНІ ТРИКУТНИКИ ================= */
/** 🔹 Заповнений трикутник (scanline-алгоритм, оптимізований під gfx_line) */
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

/* ================= КОНТУРНІ ТРИКУТНИКИ ================= */
/** 🔹 Контур трикутника (товщина 1px) */
void DrawTriangleLines(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

/** 🔹 Контур трикутника зі змінною товщиною */
void DrawTriangleLinesEx(int x1, int y1, int x2, int y2, int x3, int y3, float thickness, uint32_t color);

#ifdef __cplusplus
}
#endif
#endif // TRIANGLES_H
