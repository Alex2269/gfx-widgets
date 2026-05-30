/**
============================================================================
rects.h — Примітиви для малювання прямокутників (скалярний API)
============================================================================
🔹 Жодних Vector2! Тільки int координати
🔹 Підтримує: заповнені/контурні прямокутники, оптимізовані версії
@version 1.0 (Scalar-Only)
@license MIT
*/
#ifndef RECTS_H
#define RECTS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================= ПРЯМОКУТНИКИ ================= */
/** 🔹 Заповнений прямокутник (попіксельний, для сумісності) */
void DrawRectangle(int x, int y, int width, int height, uint32_t color);

/** 🔹 Контур прямокутника (1px) */
void DrawRectangleLines(int x, int y, int width, int height, uint32_t color);

/* ================= ОПТИМІЗОВАНІ ВЕРСІЇ ================= */
/** 🔹 Заповнений прямокутник (gfx_draw_fast_rect, подвійний буфер) */
void DrawRectangleFast(int x, int y, int width, int height, uint32_t color);

/** 🔹 Контур прямокутника (gfx_draw_fast_rect_outline) */
void DrawRectangleLinesFast(int x, int y, int width, int height, uint32_t color);

#ifdef __cplusplus
}
#endif
#endif // RECTS_H
