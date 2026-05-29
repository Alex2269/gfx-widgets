/* arrow_gfx.h */
#ifndef ARROW_GFX_H
#define ARROW_GFX_H

#include "gfx.h"
#include <stdint.h>
#include <math.h>

/* Константи кольорів для зручності */
#define GFX_RED   0xFF0000
#define GFX_GREEN 0x00FF00
#define GFX_BLUE  0x0000FF
#define GFX_BLACK 0x000000
#define GFX_WHITE 0xFFFFFF

#define DEG2RAD (3.14159265f / 180.0f)

/**
 * @brief Малює заповнений трикутник за трьома вершинами (Scanline метод).
 * @param x0, y0 Вершина 0
 * @param x1, y1 Вершина 1
 * @param x2, y2 Вершина 2
 * @param color Колір заповнення (0xRRGGBB)
 */
void gfx_draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

/**
 * @brief Малює контур рівностороннього трикутника, повернутого на заданий кут.
 * @param cx, cy Центр обертання
 * @param size Радіус описаного кола (довжина від центру до вершини)
 * @param rotation_deg Кут обертання в градусах
 * @param color Колір ліній
 */
void gfx_draw_triangle_lines_rotated(float cx, float cy, float size, float rotation_deg, uint32_t color);

/**
 * @brief Малює заповнений рівносторонній трикутник, повернутий на заданий кут.
 * @param cx, cy Центр обертання
 * @param size Радіус описаного кола
 * @param rotation_deg Кут обертання в градусах
 * @param color Колір заповнення
 */
void gfx_draw_filled_triangle_rotated(float cx, float cy, float size, float rotation_deg, uint32_t color);

/**
 * @brief Малює трикутну стрілку (Arrowhead).
 * @param bx, by Базова точка (хвіст стрілки)
 * @param angle_rad Кут напрямку в радіанах
 * @param size Розмір стрілки
 * @param color Колір
 */
void gfx_draw_arrow(float bx, float by, float angle_rad, float size, uint32_t color);

#endif /* ARROW_GFX_H */
