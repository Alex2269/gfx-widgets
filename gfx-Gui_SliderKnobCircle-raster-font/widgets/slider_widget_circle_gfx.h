/* slider_widget_circle_gfx.h */
#ifndef SLIDER_WIDGET_CIRCLE_GFX_H
#define SLIDER_WIDGET_CIRCLE_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include "color_utils.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_CIRCLE_SLIDERS_GFX 16
#define SLIDER_KNOB_RADIUS_GFX 12
#define CAPTURE_RADIUS_GFX 12

/**
 * @brief Реєструє/оновлює слайдер з круглою ручкою та малює його.
 * @param sliderIndex Індекс [0..MAX_CIRCLE_SLIDERS_GFX)
 * @param x, y, w, h Область слайдера
 * @param font, spacing Шрифт та відступ
 * @param textTop, textRight Підказки (може бути NULL)
 * @param value, minValue, maxValue Значення та діапазон
 * @param isVertical Орієнтація
 * @param baseColor Базовий колір
 */
void Gui_SliderKnobCircle_GFX(int sliderIndex, int x, int y, int w, int h,
                              RasterFont font, int spacing,
                              const char *textTop, const char *textRight,
                              float *value, float minValue, float maxValue,
                              bool isVertical, uint32_t baseColor);

/**
 * @brief Централізована функція обробки миші та малювання всіх слайдерів.
 */
void UpdateCircleKnobSlidersAndDrawGfx(RasterFont font, int spacing);

#endif // SLIDER_WIDGET_CIRCLE_GFX_H
