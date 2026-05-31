/**
============================================================================
rounded_rect.h — Заокруглені прямокутники (скалярний API)
============================================================================
🔹 Жодних Vector2! Тільки int координати
🔹 Використовує DrawCircleFilled з circles.h для кутів
🔹 Підтримує рамку з довільною товщиною
@version 1.0 (Scalar-Only + Ex)
@license MIT
*/
#ifndef ROUNDED_RECT_H
#define ROUNDED_RECT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 🔹 Заповнений заокруглений прямокутник */
void DrawRoundedRectangle(int x, int y, int w, int h, int radius, uint32_t color);

/** 🔹 Заокруглений прямокутник з рамкою (товщина + колір фону для "вирізу") */
void DrawRoundedRectangleEx(int x, int y, int w, int h, int radius, int thickness, uint32_t color, uint32_t bgColor);

/** 🔹 Контур заокругленого прямокутника (1px) */
void DrawRoundedRectangleLines(int x, int y, int w, int h, int radius, uint32_t color);

/** 🔹 Контур заокругленого прямокутника (з thickness товщиною) */
void DrawRoundedRectangleLinesEx(int x, int y, int w, int h, int radius, float thickness, uint32_t color);

#ifdef __cplusplus
}
#endif
#endif // ROUNDED_RECT_H
