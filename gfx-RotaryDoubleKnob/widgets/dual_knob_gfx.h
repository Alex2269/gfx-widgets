/* dual_knob_gfx.h */
#ifndef DUAL_KNOB_GFX_H
#define DUAL_KNOB_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_DUAL_KNOBS_GFX 5

/**
 * @brief Структура подвійного регулятора (Vernier Knob).
 */
typedef struct {
    int x, y;
    float outerRadius;
    float innerRadius;
    
    /* Кути (накопичені) */
    float outerAngle;
    float innerAngle;
    float prevOuterAngle;
    float prevInnerAngle;
    
    int64_t value; /* Результат обертання */
    
    bool isOuterDragging;
    bool isInnerDragging;
} DualKnobGfx;

/**
 * @brief Ініціалізує подвійний регулятор.
 * @param radius Радіус зовнішнього кільця.
 */
DualKnobGfx InitDualKnobGfx(int x, int y, float outerRadius);

/**
 * @brief Оновлює стан та малює регулятор.
 * @return 1, якщо значення змінилося, інакше 0.
 */
int UpdateAndDrawDualKnobGfx(DualKnobGfx *knob, RasterFont font, bool isActive);

#endif // DUAL_KNOB_GFX_H
