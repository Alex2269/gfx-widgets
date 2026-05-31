/*
 g u*i_slider_spinner_gfx.h
 ✅ Повністю розділені блоки: WHEEL | BUTTONS | KNOB
 ✅ Незалежні прапорці enable/invert для кожного типу вводу
 ✅ Виправлено: слайдери не блокують один одного, напрямок drag інтуїтивний
 */

#ifndef GUI_SLIDER_SPINNER_GFX_H
#define GUI_SLIDER_SPINNER_GFX_H

#include <stdint.h>
#include <stdbool.h>
#include "glyphs.h"
#include "gfx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SPINNERS_GFX 16

typedef enum { GUI_SPINNER_GFX_INT, GUI_SPINNER_GFX_FLOAT } GuiSpinnerGfxValueType;
typedef enum { GUI_SPINNER_GFX_HORIZONTAL, GUI_SPINNER_GFX_VERTICAL } GuiSpinnerGfxOrientation;
typedef enum { ARROW_GFX_LEFT, ARROW_GFX_RIGHT, ARROW_GFX_UP, ARROW_GFX_DOWN } ArrowDirectionGfx;

typedef struct {
    bool isHeld;
    double holdStartTime;
    double lastUpdateTime;
    double accumulatedTime;
} HoldStateGfx;

/**
@brief Віджет слайдера/спінера з окремим керуванням кожним типом вводу.
@param wheelEnable/invert  Керування колесом миші
@param btnEnable/invert    Керування кнопками
@param knobEnable/invert   Керування перетягуванням ручки (drag)
*/
int Gui_SliderSpinner_GFX(int id, int centerX, int centerY, int width, int height,
                          const char *textLeft, const char *textRight,
                          void *value, void *minValue, void *maxValue,
                          float step, GuiSpinnerGfxValueType valueType,
                          GuiSpinnerGfxOrientation orientation,
                          uint32_t baseColor, RasterFont font, int spacing,
                          bool showButtons);

#ifdef __cplusplus
}
#endif
#endif // GUI_SLIDER_SPINNER_GFX_H
