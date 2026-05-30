/**
============================================================================
rounded_rect.c — Реалізація заокруглених прямокутників
============================================================================
🔹 Використовує DrawCircleFilled з circles.h для кутів (scanline, O(radius))
🔹 Техніка рамки: Outer Fill → Inner Cutout (найшвидший метод для X11 без альфи)
🔹 Публічний API: тільки int координати
*/
#include "rounded_rect.h"
#include "circles.h"   // 🔹 DrawCircleFilled
#include "gfx.h"       // 🔹 gfx_draw_fast_rect, gfx_set_color_uint32
#include <math.h>

/* ============================================================================
🔹 ЗАПОВНЕНИЙ ЗАОКРУГЛЕНИЙ ПРЯМОКУТНИК
============================================================================ */
void DrawRoundedRectangle(int x, int y, int w, int h, int radius, uint32_t color) {
    if (w <= 0 || h <= 0) return;
    
    /* Обмежуємо радіус половиною меншої сторони */
    int maxR = (w < h) ? w / 2 : h / 2;
    if (radius > maxR) radius = maxR;
    if (radius < 0) radius = 0;

    /* 1️⃣ Центральний прямокутник + бічні смуги */
    gfx_draw_fast_rect(x + radius, y, w - 2 * radius, h, color);
    gfx_draw_fast_rect(x, y + radius, radius, h - 2 * radius, color);
    gfx_draw_fast_rect(x + w - radius, y + radius, radius, h - 2 * radius, color);
    
    /* 2️⃣ 4 кути — використовуємо DrawCircleFilled з circles.h */
    DrawCircleFilled(x + radius,         y + radius,          radius, color);  // Top-Left
    DrawCircleFilled(x + w - radius,     y + radius,          radius, color);  // Top-Right
    DrawCircleFilled(x + radius,         y + h - radius,      radius, color);  // Bottom-Left
    DrawCircleFilled(x + w - radius,     y + h - radius,      radius, color);  // Bottom-Right
}

/* ============================================================================
🔹 ЗАОКРУГЛЕНИЙ ПРЯМОКУТНИК З РАМКОЮ (Ex)
============================================================================ */
void DrawRoundedRectangleEx(int x, int y, int w, int h, int radius, int thickness, uint32_t color, uint32_t bgColor) {
    if (w <= 0 || h <= 0 || thickness <= 0) return;
    
    int maxR = (w < h) ? w / 2 : h / 2;
    if (radius > maxR) radius = maxR;
    if (thickness > radius) thickness = radius;

    /* 🔹 Крок 1: Малюємо зовнішню фігуру (колір рамки) */
    DrawRoundedRectangle(x, y, w, h, radius, color);

    /* 🔹 Крок 2: "Вирізаємо" внутрішню частину (колір фону) */
    int ix = x + thickness;
    int iy = y + thickness;
    int iw = w - 2 * thickness;
    int ih = h - 2 * thickness;
    int ir = radius - thickness;

    /* Якщо товщина занадто велика, внутрішня область зникає */
    if (iw <= 0 || ih <= 0) return;
    
    int maxIR = (iw < ih) ? iw / 2 : ih / 2;
    if (ir > maxIR) ir = maxIR;

    DrawRoundedRectangle(ix, iy, iw, ih, ir, bgColor);
}

/* ============================================================================
🔹 КОНТУР ЗАОКРУГЛЕННОГО ПРЯМОКУТНИКА (1px)
============================================================================ */
void DrawRoundedRectangleLines(int x, int y, int w, int h, int radius, uint32_t color) {
    if (w <= 0 || h <= 0) return;
    
    int maxR = (w < h) ? w / 2 : h / 2;
    if (radius > maxR) radius = maxR;
    if (radius < 0) radius = 0;

    gfx_set_color_uint32(color);

    /* Прямі ділянки */
    gfx_line(x + radius, y, x + w - radius, y);                       // Top
    gfx_line(x + radius, y + h, x + w - radius, y + h);               // Bottom
    gfx_line(x, y + radius, x, y + h - radius);                       // Left
    gfx_line(x + w, y + radius, x + w, y + h - radius);               // Right

    /* Дуги — малюємо хордами з кроком 4 пікселі для швидкості */
    const int arc_step = 4;
    for (int i = 0; i <= radius; i += arc_step) {
        int dx = (int)sqrtf((float)radius*radius - (float)i*i);
        int next_dx = (i + arc_step <= radius) 
                    ? (int)sqrtf((float)radius*radius - (float)(i+arc_step)*(i+arc_step)) 
                    : 0;

        /* Top-Left */
        gfx_line(x + radius - dx, y + i, x + radius - next_dx, y + i + arc_step);
        /* Top-Right */
        gfx_line(x + w - radius + dx, y + i, x + w - radius + next_dx, y + i + arc_step);
        /* Bottom-Left */
        gfx_line(x + radius - dx, y + h - i, x + radius - next_dx, y + h - i - arc_step);
        /* Bottom-Right */
        gfx_line(x + w - radius + dx, y + h - i, x + w - radius + next_dx, y + h - i - arc_step);
    }
}
