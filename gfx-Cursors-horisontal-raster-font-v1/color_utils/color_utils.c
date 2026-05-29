/*
 * color_utils.c
 * Реалізація утиліт для роботи з кольорами (uint32_t 0xRRGGBB)
 */

#include "color_utils.h"

/* ================= КОНВЕРТАЦІЯ КОЛЬОРІВ ================= */

void RGBtoFloatComponents(uint32_t color, float *r, float *g, float *b) {
    *r = ((color >> 16) & 0xFF) / 255.0f;
    *g = ((color >> 8) & 0xFF) / 255.0f;
    *b = (color & 0xFF) / 255.0f;
}

uint32_t FloatComponentsToRGB(float r, float g, float b) {
    // Clamp до [0..1] перед конвертацією
    if (r < 0.0f) r = 0.0f; if (r > 1.0f) r = 1.0f;
    if (g < 0.0f) g = 0.0f; if (g > 1.0f) g = 1.0f;
    if (b < 0.0f) b = 0.0f; if (b > 1.0f) b = 1.0f;

    // +0.5f для коректного округлення при касті в int
    uint32_t R = (uint32_t)(r * 255.0f + 0.5f) & 0xFF;
    uint32_t G = (uint32_t)(g * 255.0f + 0.5f) & 0xFF;
    uint32_t B = (uint32_t)(b * 255.0f + 0.5f) & 0xFF;
    return (R << 16) | (G << 8) | B;
}

/* ================= ЯСКРАВІСТЬ ТА КОНТРАСТ ================= */

float GetLuminance(uint32_t color) {
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;
    // Формула ITU-R BT.709 для сприйняття яскравості людським оком
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

uint32_t GetContrastColor(uint32_t color) {
    return (GetLuminance(color) > 0.5f) ? BLACK : WHITE;
}

bool IsColorDark(uint32_t color) {
    return GetLuminance(color) < 0.5f;
}

/* ================= MANIPULЯЦІЇ З КОЛЬОРОМ ================= */

uint32_t ChangeSaturation(uint32_t color, float saturationScale) {
    float r, g, b;
    RGBtoFloatComponents(color, &r, &g, &b);

    float cMax = fmaxf(r, fmaxf(g, b));
    float cMin = fminf(r, fminf(g, b));
    float delta = cMax - cMin;

    float h = 0.0f;
    // Розрахунок Hue (0..360)
    if (delta > 0.0001f) {
        if (cMax == r) {
            h = fmodf((g - b) / delta, 6.0f);
            // ⚠️ КРИТИЧНЕ ВИПРАВЛЕННЯ: fmodf може повернути від'ємне значення,
            // що ламає розрахунок і перетворює червоний на зелений.
            if (h < 0.0f) h += 6.0f; 
        } else if (cMax == g) {
            h = (b - r) / delta + 2.0f;
        } else {
            h = (r - g) / delta + 4.0f;
        }
        h *= 60.0f;
    }

    float s = (cMax < 0.0001f) ? 0.0f : delta / cMax;
    float v = cMax;

    s *= saturationScale;
    if (s > 1.0f) s = 1.0f;
    if (s < 0.0f) s = 0.0f;

    // Розрахунок RGB з HSV
    float cVal = v * s;
    
    // ⚠️ КРИТИЧНЕ ВИПРАВЛЕННЯ: захист від похибок fmodf при xVal
    float hNorm = fmodf(h / 60.0f, 2.0f);
    if (hNorm < 0.0f) hNorm += 2.0f;
    
    float xVal = cVal * (1.0f - fabsf(hNorm - 1.0f));
    float m = v - cVal;

    float nr = 0.0f, ng = 0.0f, nb = 0.0f;

    if      (h >= 0.0f   && h < 60.0f)  { nr = cVal; ng = xVal; nb = 0.0f; }
    else if (h >= 60.0f  && h < 120.0f) { nr = xVal; ng = cVal; nb = 0.0f; }
    else if (h >= 120.0f && h < 180.0f) { nr = 0.0f;  ng = cVal; nb = xVal; }
    else if (h >= 180.0f && h < 240.0f) { nr = 0.0f;  ng = xVal; nb = cVal; }
    else if (h >= 240.0f && h < 300.0f) { nr = xVal; ng = 0.0f;  nb = cVal; }
    else                                { nr = cVal; ng = 0.0f;  nb = xVal; }

    nr += m; ng += m; nb += m;
    return FloatComponentsToRGB(nr, ng, nb);
}

uint32_t ChangeBrightness(uint32_t color, float brightnessScale) {
    float r = ((color >> 16) & 0xFF) * brightnessScale;
    float g = ((color >> 8) & 0xFF) * brightnessScale;
    float b = (color & 0xFF) * brightnessScale;

    if (r > 255.0f) r = 255.0f; if (r < 0.0f) r = 0.0f;
    if (g > 255.0f) g = 255.0f; if (g < 0.0f) g = 0.0f;
    if (b > 255.0f) b = 255.0f; if (b < 0.0f) b = 0.0f;

    return ((uint8_t)r << 16) | ((uint8_t)g << 8) | (uint8_t)b;
}

uint32_t InvertColor(uint32_t color) {
    uint8_t r = 255 - ((color >> 16) & 0xFF);
    uint8_t g = 255 - ((color >> 8) & 0xFF);
    uint8_t b = 255 - (color & 0xFF);
    return (r << 16) | (g << 8) | b;
}

uint32_t GetContrastInvertColor(uint32_t color) {
    uint32_t invColor = InvertColor(color);

    if (IsColorDark(color)) {
        // Якщо оригінал темний → інверсія світла → робимо фон ще світлішим і менш насиченим
        invColor = ChangeSaturation(invColor, 0.35f);
        uint8_t r = (uint8_t)((((invColor >> 16) & 0xFF) + 255) / 2);
        uint8_t g = (uint8_t)((((invColor >> 8) & 0xFF) + 255) / 2);
        uint8_t b = (uint8_t)(((invColor & 0xFF) + 255) / 2);
        invColor = (r << 16) | (g << 8) | b;
    } else {
        // Якщо оригінал світлий → інверсія темна → робимо фон ще темнішим і менш насиченим
        invColor = ChangeSaturation(invColor, 0.65f);
        uint8_t r = (uint8_t)(((invColor >> 16) & 0xFF) / 2);
        uint8_t g = (uint8_t)(((invColor >> 8) & 0xFF) / 2);
        uint8_t b = (uint8_t)((invColor & 0xFF) / 2);
        invColor = (r << 16) | (g << 8) | b;
    }
    return invColor;
}
