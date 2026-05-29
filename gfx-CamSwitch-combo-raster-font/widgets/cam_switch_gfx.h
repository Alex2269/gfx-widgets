/*
 * cam_switch_gfx.h
 * Спрощений віджет rotary cam switch для X11/gfx з підтримкою миші (raylib-style polling)
 * Використовує RasterFont та uint32_t кольори (0xRRGGBB)
 */
#ifndef CAM_SWITCH_GFX_H
#define CAM_SWITCH_GFX_H

#include "gfx.h"
#include <stdint.h>
#include <stdbool.h>
#include "glyphs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Відображає та обробляє поворотний перемикач (rotary cam switch) для gfx.
 * 
 * @param channel      Індекс каналу (0-5) для збереження стану drag/angle між кадрами.
 * @param font_knob    Шрифт для підписів шкали навколо ручки.
 * @param font_value   Шрифт для відображення поточного значення під ручкою.
 * @param x_pos        X-координата центру віджета.
 * @param y_pos        Y-координата центру віджета.
 * @param tooltipTop   Текст підказки, що з'являється при наведенні (може бути NULL).
 * @param textRight    Текст праворуч від віджета (може бути NULL).
 * @param radius       Радіус ручки (в пікселях).
 * @param value        Вказівник на поточне значення (оновлюється функцією).
 * @param minValue     Мінімальне значення діапазону.
 * @param maxValue     Максимальне значення діапазону.
 * @param isActive     Прапорець активності віджета.
 * @param colorText    Колір тексту, стрілки та ділень (формат 0xRRGGBB).
 * @param logarithmic  true - логарифмічна шкала [2, 5, 10... 5000], false - лінійна.
 * 
 * @return 1 якщо значення змінилося в цьому кадрі, 0 інакше.
 */
int Gui_CamSwitch_GFX(int channel, RasterFont font_knob, RasterFont font_value,
                      int x_pos, int y_pos, const char *tooltipTop, const char *textRight,
                      float radius, float *value, float minValue, float maxValue,
                      bool isActive, uint32_t colorText, bool logarithmic);

#ifdef __cplusplus
}
#endif

#endif // CAM_SWITCH_GFX_H
