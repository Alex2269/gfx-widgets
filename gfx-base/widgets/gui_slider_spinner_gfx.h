/*
 * gui_slider_spinner_gfx.h
 * Адаптація Gui_SliderSpinner для X11/gfx з підтримкою миші (raylib-style polling)
 * Використовує RasterFont та uint32_t кольори (0xRRGGBB)
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

/* Типи значень */
typedef enum {
    GUI_SPINNER_GFX_INT,
    GUI_SPINNER_GFX_FLOAT
} GuiSpinnerGfxValueType;

/* Орієнтація */
typedef enum {
    GUI_SPINNER_GFX_HORIZONTAL,
    GUI_SPINNER_GFX_VERTICAL
} GuiSpinnerGfxOrientation;

/* Напрямок стрілки */
typedef enum {
    ARROW_GFX_LEFT,
    ARROW_GFX_RIGHT,
    ARROW_GFX_UP,
    ARROW_GFX_DOWN
} ArrowDirectionGfx;

/* Стан утримання кнопки для автоповтору */
typedef struct {
    bool isHeld;
    double holdStartTime;
    double lastUpdateTime;
    double accumulatedTime;
} HoldStateGfx;

/**
 * @brief Поєднаний віджет слайдера та спінера для gfx.
 * @return 1 якщо значення змінилося, 0 інакше.
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
