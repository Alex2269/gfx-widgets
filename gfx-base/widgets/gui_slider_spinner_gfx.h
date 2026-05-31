/* gui_slider_spinner_gfx.h */
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

typedef enum {
    GUI_SPINNER_GFX_INT,
    GUI_SPINNER_GFX_FLOAT
} GuiSpinnerGfxValueType;

typedef enum {
    GUI_SPINNER_GFX_HORIZONTAL,
    GUI_SPINNER_GFX_VERTICAL
} GuiSpinnerGfxOrientation;

typedef enum {
    ARROW_GFX_LEFT, ARROW_GFX_RIGHT, ARROW_GFX_UP, ARROW_GFX_DOWN
} ArrowDirectionGfx;

typedef struct {
    bool isHeld;
    double holdStartTime;
    double lastUpdateTime;
    double accumulatedTime;
} HoldStateGfx;

/**
@brief Поєднаний віджет слайдера та спінера для gfx.
@param value Вказівник на змінну значення (int* або float*)
@param minVal, maxVal Межі діапазону (передаються як double, кастяться всередині)
@note Для GUI_SPINNER_GFX_INT: передавай int* та цілі double-значення
@note Для GUI_SPINNER_GFX_FLOAT: передавай float* та дробові double-значення
@return 1 якщо значення змінилося, 0 інакше.
*/
int Gui_SliderSpinner_GFX(int id, int centerX, int centerY, int width, int height,
                          const char *textLeft, const char *textRight,
                          void *value, double minVal, double maxVal,  // ✅ Скалярні min/max!
                          float step, GuiSpinnerGfxValueType valueType,
                          GuiSpinnerGfxOrientation orientation,
                          uint32_t baseColor, RasterFont font, int spacing,
                          bool showButtons);

#ifdef __cplusplus
}
#endif
#endif // GUI_SLIDER_SPINNER_GFX_H
