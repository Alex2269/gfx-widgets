/*
cam_switch_gfx.h
Поворотний перемикач (rotary cam switch) для gfx
Підтримує: мишу (polling), колесо, підказки, авто-контраст, інвертовані діапазони
*/
#ifndef CAM_SWITCH_GFX_H
#define CAM_SWITCH_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include "color_utils.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Відображає та обробляє поворотний перемикач для gfx.
 * @param id Унікальний індекс [0..9] для збереження стану між кадрами
 * @param cx, cy Координати центру
 * @param radius Радіус ручки
 * @param value Вказівник на float-значення
 * @param minValue, maxValue Діапазон (підтримує інверсію: min > max)
 * @param textColor Колір тексту, шкали, стрілки (0xRRGGBB)
 * @param textTop Підказка зверху (може бути багаторядковою через \n)
 * @param textRight Текст праворуч
 * @param font Шрифт для відображення
 * @param spacing Відступ між символами
 * @return 1, якщо значення змінилося в цьому кадрі
 */
int Gui_CamSwitch_GFX(int id, int cx, int cy, float radius,
                      float *value, float minValue, float maxValue,
                      uint32_t textColor, const char *textTop, 
                      const char *textRight, RasterFont font, int spacing);

#ifdef __cplusplus
}
#endif

#endif /* CAM_SWITCH_GFX_H */
