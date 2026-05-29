/* arrow_gfx.h */
#ifndef ARROW_GFX_H
#define ARROW_GFX_H

#include "gfx.h"
#include <stdint.h>

/**
 * @brief Малює заповнений трикутник за координатами вершин.
 */
void gfx_draw_filled_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

/**
 * @brief Малює контур трикутника.
 */
void gfx_draw_triangle_outline(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

/**
 * @brief ✅ НОВА: Малює контур рівностороннього трикутника з обертанням.
 * @param cx, cy Центр обертання
 * @param size Довжина сторони трикутника
 * @param rotation_deg Кут повороту в градусах
 * @param color Колір контуру
 */
void gfx_draw_triangle_outline_rotated(float cx, float cy, float size, float rotation_deg, uint32_t color);

/**
 * @brief Малює стрілку (трикутник), спрямовану під заданим кутом.
 */
void gfx_draw_arrow(float cx, float cy, float angle_deg, float size, uint32_t color);

/**
 * @brief Малює рівносторонній трикутник, повернутий навколо центру.
 */
void gfx_draw_rotated_triangle(float cx, float cy, float size, float rotation_deg, uint32_t color);

#endif /* ARROW_GFX_H */
