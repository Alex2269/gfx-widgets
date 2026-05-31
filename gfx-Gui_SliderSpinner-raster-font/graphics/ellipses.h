/**
============================================================================
ellipses.h — Примітиви еліпсів та дуг (скалярний API)
============================================================================
🔹 Жодних Vector2! Тільки int координати
🔹 Підтримує: заповнені еліпси, контури, дуги, сектори (pie)
🔹 Алгоритми: Midpoint Ellipse + Scanline Fill (оптимізовано під gfx)
@version 1.0 (Scalar-Only)
@license MIT
*/
#ifndef ELLIPSES_H
#define ELLIPSES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================= ЕЛІПСИ ================= */
/** 🔹 Заповнений еліпс (scanline-алгоритм, O(radius) ліній) */
void DrawEllipseFilled(int cx, int cy, int rx, int ry, uint32_t color);

/** 🔹 Контур еліпса (1px, Midpoint Ellipse Algorithm) */
void DrawEllipseLines(int cx, int cy, int rx, int ry, uint32_t color);

/** 🔹 Контур еліпса зі змінною товщиною */
void DrawEllipseLinesEx(int cx, int cy, int rx, int ry, float thickness, uint32_t color);

/* ================= ДУГИ / СЕКТОРИ ================= */
/** 🔹 Дуга еліпса (контур, кути в градусах, за годинниковою стрілкою) */
void DrawArc(int cx, int cy, int radius, float startAngle, float endAngle, uint32_t color);

/** 🔹 Заповнена дуга / сектор (pie slice) */
void DrawArcFilled(int cx, int cy, int rx, int ry, float startAngle, float endAngle, uint32_t color);

/** 🔹 Універсальна дуга: контур/заповнення/товщина */
void DrawArcEx(int cx, int cy, int rx, int ry, float startAngle, float endAngle, 
               float thickness, uint32_t color, bool fill);

#ifdef __cplusplus
}
#endif
#endif // ELLIPSES_H
