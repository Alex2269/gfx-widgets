/* slider_knob_diamond_gfx.h */
#ifndef SLIDER_KNOB_DIAMOND_GFX_H
#define SLIDER_KNOB_DIAMOND_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include "color_utils.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_DIAMOND_SLIDERS_GFX 16
#define KNOB_DIAMOND_SIZE_GFX 12  // Половина діагоналі ромба (радіус)
#define CAPTURE_RADIUS_GFX 15     // Зона захоплення

/**
 * @brief Реєструє/оновлює слайдер з ромбоподібною ручкою.
 * ️ Викликає UpdateDiamondKnobSlidersAndDrawGfx всередині для миттєвого відображення.
 */
void Gui_SliderKnobDiamond_GFX(int sliderIndex, int x, int y, int w, int h,
                               RasterFont font, int spacing,
                               const char *textTop, const char *textRight,
                               float *value, float minValue, float maxValue,
                               bool isVertical, uint32_t baseColor);

/**
 * @brief Централізована функція обробки миші та малювання всіх слайдерів.
 */
void UpdateDiamondKnobSlidersAndDrawGfx(RasterFont font, int spacing);

#endif // SLIDER_KNOB_DIAMOND_GFX_H
