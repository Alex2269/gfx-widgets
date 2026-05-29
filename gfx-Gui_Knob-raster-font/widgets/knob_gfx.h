/* knob_gfx.h */
#ifndef KNOB_GFX_H
#define KNOB_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Малює поворотний регулятор (knob) з кольоровими секторами.
 * @param id Унікальний індекс [0..9]
 * @param cx, cy Координати центру
 * @param radius Радіус регулятора
 * @param value Вказівник на значення
 * @param minValue, maxValue Діапазон
 * @param textColor Колір тексту, шкали та стрілки
 * @param textTop Підказка зверху (може бути NULL)
 * @param font Шрифт для тексту
 * @return 1, якщо значення змінилося
 */
int Gui_Knob_GFX(int id, int cx, int cy, float radius,
                 float *value, float minValue, float maxValue,
                 uint32_t textColor, const char *textTop, RasterFont font);

#endif /* KNOB_GFX_H */
