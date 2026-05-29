/*
 * color_utils.h
 * Утиліти для роботи з кольорами у форматі uint32_t (0xRRGGBB)
 * Підтримує: контраст, інверсію, насиченість (HSV), яскравість
 */
#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include "color.h"      // Базові константи кольорів (WHITE, BLACK, тощо)
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================= КОНВЕРТАЦІЯ КОЛЬОРІВ ================= */
void RGBtoFloatComponents(uint32_t color, float *r, float *g, float *b);
uint32_t FloatComponentsToRGB(float r, float g, float b);

/* ================= ЯСКРАВІСТЬ ТА КОНТРАСТ ================= */
float GetLuminance(uint32_t color);
uint32_t GetContrastColor(uint32_t color);
bool IsColorDark(uint32_t color);

/* ================= MANIPULЯЦІЇ З КОЛЬОРОМ ================= */

/**
 * @brief Змінює насиченість (saturation) кольору в HSV-просторі
 * @param color Вхідний колір (0xRRGGBB)
 * @param saturationScale Масштаб насиченості:
 *        - < 1.0 : зменшує насиченість (більше сірого)
 *        - = 1.0 : без змін
 *        - > 1.0 : збільшує насиченість (до максимуму 1.0)
 * @return Новий колір із зміненою насиченістю
 */
uint32_t ChangeSaturation(uint32_t color, float saturationScale);

/**
 * @brief Змінює яскравість кольору (множення RGB-компонент)
 * @param brightnessScale Масштаб яскравості (напр. 0.8f для затемнення, 1.2f для освітлення)
 */
uint32_t ChangeBrightness(uint32_t color, float brightnessScale);

uint32_t InvertColor(uint32_t color);
uint32_t GetContrastInvertColor(uint32_t color);

/* ================= ЗРУЧНІ ХЕЛПЕРИ ДЛЯ ВІДЖЕТІВ ================= */

/**
 * @brief Збільшує насиченість на 20% (для hover-ефектів)
 */
static inline uint32_t ColorHover(uint32_t color) {
    return ChangeSaturation(color, 1.2f);
}

/**
 * @brief Збільшує насиченість на 50% (для active-ефектів)
 */
static inline uint32_t ColorActive(uint32_t color) {
    return ChangeSaturation(color, 1.5f);
}

/**
 * @brief Зменшує насиченість до 40% (для disabled/неактивних елементів)
 */
static inline uint32_t ColorDisabled(uint32_t color) {
    return ChangeSaturation(color, 0.4f);
}

/**
 * @brief Робить колір світлішим (імітація "Fade" без альфа-каналу)
 */
static inline uint32_t ColorBrighten(uint32_t color, float factor) {
    float r = ((color >> 16) & 0xFF) * factor;
    float g = ((color >> 8) & 0xFF) * factor;
    float b = (color & 0xFF) * factor;
    if (r > 255.0f) r = 255.0f; if (g > 255.0f) g = 255.0f; if (b > 255.0f) b = 255.0f;
    return ((uint8_t)r << 16) | ((uint8_t)g << 8) | (uint8_t)b;
}

/**
 * @brief Робить колір темнішим (для pressed-стану)
 */
static inline uint32_t ColorDarken(uint32_t color, float factor) {
    float r = ((color >> 16) & 0xFF) * factor;
    float g = ((color >> 8) & 0xFF) * factor;
    float b = (color & 0xFF) * factor;
    if (r < 0.0f) r = 0.0f; if (g < 0.0f) g = 0.0f; if (b < 0.0f) b = 0.0f;
    return ((uint8_t)r << 16) | ((uint8_t)g << 8) | (uint8_t)b;
}

#ifdef __cplusplus
}
#endif

#endif /* COLOR_UTILS_H */
