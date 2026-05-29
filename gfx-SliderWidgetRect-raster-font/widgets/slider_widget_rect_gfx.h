/* slider_widget_rect_gfx.h */
#ifndef SLIDER_WIDGET_RECT_GFX_H
#define SLIDER_WIDGET_RECT_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_RECT_SLIDERS_GFX 16
#define KNOB_WIDTH_GFX   8
#define KNOB_HEIGHT_GFX  12
#define CAPTURE_PAD_GFX  8

/**
 * @brief Слайдер з прямокутною ручкою для gfx.
 * @param sliderIndex Унікальний індекс [0..MAX_RECT_SLIDERS_GFX)
 * @param x, y, width, height Область слайдера
 * @param value Вказівник на float-значення
 * @param minValue, maxValue Діапазон значень
 * @param isVertical true = вертикальний, false = горизонтальний
 * @param baseColor Базовий колір (0xRRGGBB)
 * @param textTop Підказка зверху (може бути NULL)
 * @param textRight Підказка праворуч (може бути NULL)
 */
void RegisterRectKnobSliderGfx(int sliderIndex, int x, int y, int width, int height,
                               float *value, float minValue, float maxValue,
                               bool isVertical, uint32_t baseColor,
                               const char *textTop, const char *textRight);

/**
 * @brief Оновлює стан і малює всі зареєстровані слайдери.
 * @param font Шрифт для тексту
 * @param spacing Відступ між символами
 */
void UpdateRectKnobSlidersAndDrawGfx(RasterFont font, int spacing);

#endif // SLIDER_WIDGET_RECT_GFX_H
