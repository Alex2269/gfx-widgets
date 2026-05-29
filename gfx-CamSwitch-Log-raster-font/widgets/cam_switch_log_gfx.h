/*
cam_switch_log_gfx.h
Поворотний перемикач з логарифмічною/лінійною шкалою для gfx
Підтримує: мишу (polling), колесо, підказки, авто-контраст
*/
#ifndef CAM_SWITCH_LOG_GFX_H
#define CAM_SWITCH_LOG_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include "color_utils.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Відображає поворотний перемикач з логарифмічною або лінійною шкалою.
 * @param id Унікальний індекс [0..9] для збереження стану між кадрами
 * @param cx, cy Координати центру
 * @param radius Радіус ручки
 * @param value Вказівник на float-значення
 * @param minValue, maxValue Діапазон (ігнорується при logarithmic=true)
 * @param logarithmic true = логарифмічна шкала {2,5,10...5000}, false = лінійна
 * @param textColor Колір тексту, шкали, стрілки (0xRRGGBB)
 * @param textTop Підказка зверху (може бути багаторядковою через \n)
 * @param textRight Текст праворуч
 * @param font Шрифт для відображення
 * @param spacing Відступ між символами
 * @return 1, якщо значення змінилося в цьому кадрі
 */
int Gui_CamSwitch_Log_GFX(int id, int cx, int cy, float radius,
                          float *value, float minValue, float maxValue,
                          bool logarithmic, uint32_t textColor,
                          const char *textTop, const char *textRight,
                          RasterFont font, int spacing);

#ifdef __cplusplus
}
#endif

#endif /* CAM_SWITCH_LOG_GFX_H */
