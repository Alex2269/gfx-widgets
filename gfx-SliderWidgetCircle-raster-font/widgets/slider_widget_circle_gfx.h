/* slider_widget_circle_gfx.h */
#ifndef SLIDER_WIDGET_CIRCLE_GFX_H
#define SLIDER_WIDGET_CIRCLE_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include "color_utils.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_CIRCLE_SLIDERS_GFX 16
#define SLIDER_KNOB_RADIUS_GFX 5    // Радіус ручки
#define CAPTURE_RADIUS_GFX 12       // Зона захоплення мишею

/**
 * @brief Структура слайдера з круглою ручкою для gfx.
 */
typedef struct {
    int x, y, width, height;    // Область слайдера
    float *value;               // Вказівник на значення
    float minValue, maxValue;   // Діапазон
    bool isVertical;            // Орієнтація
    uint32_t baseColor;         // Базовий колір
    bool isActive;              // Чи захоплений мишею
    bool used;                  // Чи використовується слот
    const char *textTop;        // Підказка зверху
    const char *textRight;      // Підказка праворуч
} CircleSliderGfx;

/**
 * @brief Реєструє або оновлює слайдер.
 * @param sliderIndex Індекс [0..MAX_CIRCLE_SLIDERS_GFX)
 */
void RegisterCircleKnobSliderGfx(int sliderIndex, int x, int y, int width, int height,
                                 float *value, float minValue, float maxValue,
                                 bool isVertical, uint32_t baseColor,
                                 const char *textTop, const char *textRight);

/**
 * @brief Оновлює стан і малює всі слайдери.
 * Викликати в кожному кадрі після gfx_clear().
 */
void UpdateCircleKnobSlidersAndDrawGfx(RasterFont font, int spacing);

#endif // SLIDER_WIDGET_CIRCLE_GFX_H
