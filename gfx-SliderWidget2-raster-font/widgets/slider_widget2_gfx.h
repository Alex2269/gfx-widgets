/* slider_widget2_gfx.h */
#ifndef SLIDER_WIDGET2_GFX_H
#define SLIDER_WIDGET2_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include "color_utils.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Реєструє слайдер із прямокутною ручкою або оновлює його параметри.
 * ️ Викликає UpdateSlidersAndDrawGfx всередині для миттєвого відображення.
 * @param sliderIndex Індекс [0..15]
 * @param x, y, w, h Область слайдера
 * @param font, spacing Шрифт та відступ
 * @param textTop, textRight Підказки (може бути NULL)
 * @param value, minVal, maxVal Значення та діапазон
 * @param isVertical Орієнтація
 * @param baseColor Базовий колір
 */
void RegisterSlider2Gfx(int sliderIndex, int x, int y, int w, int h,
                        RasterFont font, int spacing,
                        const char *textTop, const char *textRight,
                        float *value, float minVal, float maxVal,
                        bool isVertical, uint32_t baseColor);

/**
 * @brief Централізована обробка миші та малювання всіх слайдерів.
 */
void UpdateSlidersAndDraw2Gfx(RasterFont font, int spacing);

#endif // SLIDER_WIDGET2_GFX_H
