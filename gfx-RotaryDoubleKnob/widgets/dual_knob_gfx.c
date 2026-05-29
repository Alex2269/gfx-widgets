/* dual_knob_gfx.c */
#include "dual_knob_gfx.h"
#include "graphics.h"
#include "gfx.h"
#include "color_utils.h"
#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979323846f
#define INNER_TO_OUTER_RATIO 10.0f
#define INNER_RADIUS_RATIO 0.45f

/* ================= ДОПОМІЖНІ ГРАФІЧНІ ФУНКЦІЇ ================= */

/* Малювання заповненого кола (scanline метод, швидкий для gfx) */
static void gfx_draw_filled_circle(int cx, int cy, int r, uint32_t color) {
    if (r <= 0) return;
    gfx_set_color_uint32(color);
    for (int y = -r; y <= r; y++) {
        int dx = (int)sqrtf((float)r*r - (float)y*y);
        gfx_line(cx - dx, cy + y, cx + dx, cy + y);
    }
}

/* Малювання товстої лінії (імітація DrawLineEx) */
static void gfx_draw_thick_line(int x1, int y1, int x2, int y2, int thickness, uint32_t color) {
    if (thickness <= 1) {
        gfx_set_color_uint32(color);
        gfx_line(x1, y1, x2, y2);
        return;
    }
    
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrt(dx*dx + dy*dy);
    if (len == 0) return;
    
    /* Нормаль до лінії */
    float nx = -dy / len;
    float ny = dx / len;
    
    gfx_set_color_uint32(color);
    for (int t = -thickness/2; t <= thickness/2; t++) {
        int ox = (int)(nx * t);
        int oy = (int)(ny * t);
        gfx_line(x1 + ox, y1 + oy, x2 + ox, y2 + oy);
    }
}

/* Нормалізація кута для малювання (0..360) */
static float AngleForRender(float angle) {
    float a = fmodf(angle, 360.0f);
    if (a < 0) a += 360.0f;
    return a;
}

/* ================= ГОЛОВНА ФУНКЦІЯ ================= */

DualKnobGfx InitDualKnobGfx(int x, int y, float outerRadius) {
    DualKnobGfx knob;
    knob.x = x;
    knob.y = y;
    knob.outerRadius = outerRadius;
    knob.innerRadius = outerRadius * INNER_RADIUS_RATIO;
    
    knob.outerAngle = 0.0f;
    knob.innerAngle = 0.0f;
    knob.prevOuterAngle = 0.0f;
    knob.prevInnerAngle = 0.0f;
    knob.value = 0;
    knob.isOuterDragging = false;
    knob.isInnerDragging = false;
    return knob;
}

int UpdateAndDrawDualKnobGfx(DualKnobGfx *knob, RasterFont font, bool isActive) {
    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();
    
    int changed = 0;

    /* 1. Логіка миші (обробка перетягування) */
    if (isActive) {
        float dist = sqrt(pow(mx - knob->x, 2) + pow(my - knob->y, 2));
        bool mouseInInner = dist < knob->innerRadius;
        bool mouseInOuter = (dist >= knob->innerRadius) && (dist <= knob->outerRadius);

        /* Початок перетягування */
        if (gfx_is_mouse_pressed(GFX_MOUSE_LEFT)) {
            if (mouseInInner) {
                knob->isInnerDragging = true;
                knob->prevInnerAngle = atan2f(my - knob->y, mx - knob->x) * (180.0f / PI);
                knob->isOuterDragging = false;
            } else if (mouseInOuter) {
                knob->isOuterDragging = true;
                knob->prevOuterAngle = atan2f(my - knob->y, mx - knob->x) * (180.0f / PI);
                knob->isInnerDragging = false;
            }
        }

        /* Завершення перетягування */
        if (gfx_is_mouse_released(GFX_MOUSE_LEFT)) {
            knob->isInnerDragging = false;
            knob->isOuterDragging = false;
        }

        /* Обробка руху */
        float deltaAngle = 0;
        
        if (knob->isOuterDragging) {
            float currentOuterAngle = atan2f(my - knob->y, mx - knob->x) * (180.0f / PI);
            deltaAngle = currentOuterAngle - knob->prevOuterAngle;
            
            /* Корекція переходу 0/360 */
            if (deltaAngle > 180.0f) deltaAngle -= 360.0f;
            if (deltaAngle < -180.0f) deltaAngle += 360.0f;
            
            knob->outerAngle += deltaAngle;
            knob->innerAngle += deltaAngle * INNER_TO_OUTER_RATIO;
            knob->prevOuterAngle = currentOuterAngle;
            changed = 1;
        } 
        else if (knob->isInnerDragging) {
            float currentInnerAngle = atan2f(my - knob->y, mx - knob->x) * (180.0f / PI);
            deltaAngle = currentInnerAngle - knob->prevInnerAngle;
            
            if (deltaAngle > 180.0f) deltaAngle -= 360.0f;
            if (deltaAngle < -180.0f) deltaAngle += 360.0f;
            
            knob->innerAngle += deltaAngle;
            knob->outerAngle += deltaAngle / INNER_TO_OUTER_RATIO;
            knob->prevInnerAngle = currentInnerAngle;
            changed = 1;
        }
        
        /* Розрахунок значення */
        int64_t newValue = (int64_t)(knob->innerAngle * (100.0f / 360.0f));
        if (newValue != knob->value) {
            knob->value = newValue;
            changed = 1;
        }
    }

    /* ================= 2. МАЛЮВАННЯ (Виправлений порядок) ================= */
    
    /* КРОК 1: Зовнішній фон (Світло-сірий) */
    gfx_draw_filled_circle(knob->x, knob->y, (int)knob->outerRadius, 0xD3D3D3); /* LIGHTGRAY */
    gfx_draw_filled_circle(knob->x, knob->y, (int)knob->outerRadius - 5, 0x404040); /* DARKGRAY */
    
    /* 
       КРОК 2: Індикатор Зовнішнього (Синій)
       ✅ Важливо: Малюємо синю стрілку ЗРАЗУ після зовнішнього фону.
       Це дозволяє внутрішньому диску (який малюється далі) перекривати 
       початок синьої стрілки.
    */
    float outerRenderAngle = (AngleForRender(knob->outerAngle) - 90.0f) * (PI / 180.0f);
    int ox = knob->x + (int)(cosf(outerRenderAngle) * (knob->outerRadius - 10));
    int oy = knob->y + (int)(sinf(outerRenderAngle) * (knob->outerRadius - 10));
    gfx_draw_thick_line(knob->x, knob->y, ox, oy, 4, 0x0000FF); /* BLUE */

    /* 
       КРОК 3: Внутрішній фон (Сірий/Темний)
       ✅ Цей диск малюється ПОВЕРХ синьої стрілки.
       Тепер синя стрілка виглядає так, ніби виходить з-під диска.
    */
    gfx_draw_filled_circle(knob->x, knob->y, (int)knob->innerRadius, 0x808080); /* GRAY */
    gfx_draw_filled_circle(knob->x, knob->y, (int)knob->innerRadius - 3, 0x404040); /* DARKGRAY */

    /* КРОК 4: Індикатор Внутрішнього (Червоний) */
    float innerRenderAngle = (AngleForRender(knob->innerAngle) - 90.0f) * (PI / 180.0f);
    int ix = knob->x + (int)(cosf(innerRenderAngle) * (knob->innerRadius - 7));
    int iy = knob->y + (int)(sinf(innerRenderAngle) * (knob->innerRadius - 7));
    gfx_draw_thick_line(knob->x, knob->y, ix, iy, 3, 0xFF0000); /* RED */

    /* Вивід значення */
    char valText[40];
    snprintf(valText, sizeof(valText), "%lld", (long long)knob->value);
    DrawTextScaled(font, knob->x + knob->outerRadius + 15, knob->y - 10, 
                   valText, 2, 1, BLACK);

    return changed;
}
