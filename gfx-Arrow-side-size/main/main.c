/* main.c */
#include "main.h"
#include "arrow_gfx.h"
#include "all_font.h"
#include "glyphs.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

int spacing = 2;

int main(void) {
    const int W = 600, H = 450;
    gfx_open(W, H, "GFX Arrow & Triangle Demo");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);

    float angle = 0.0f;

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        /* Анімація обертання */
        angle += 1.0f;
        if (angle > 360.0f) angle -= 360.0f;

        gfx_clear();

        /* 1. Стрілки під різними кутами */
        gfx_draw_arrow(150, 100, 0.0f,   40.0f, 0xFFFF00); /* Праворуч */
        gfx_draw_arrow(150, 150, 90.0f,  40.0f, 0x00FF00); /* Вниз */
        gfx_draw_arrow(150, 200, 180.0f, 40.0f, 0xFF0000); /* Ліворуч */
        gfx_draw_arrow(150, 250, 270.0f, 40.0f, 0x0000FF); /* Вгору */

        /* 2. Обертаний рівносторонній трикутник */
        gfx_draw_rotated_triangle(350, 175, 60.0f, angle, 0x87CEEB); /* SKYBLUE */
        
        /* Контур навколо нього (трохи більший) */
        float rad = angle * (PI / 180.0f);
        float size = 65.0f;
        float h = size * sqrtf(3.0f) / 2.0f;
        float px[3] = { 0.0f, -size/2.0f, size/2.0f };
        float py[3] = { -2.0f/3.0f * h, 1.0f/3.0f * h, 1.0f/3.0f * h };
        float cr = cosf(rad), sr = sinf(rad);
        int vx[3], vy[3];
        for (int i = 0; i < 3; i++) {
            vx[i] = (int)(350 + px[i]*cr - py[i]*sr);
            vy[i] = (int)(175 + px[i]*sr + py[i]*cr);
        }
        gfx_draw_triangle_outline(vx[0], vy[0], vx[1], vy[1], vx[2], vy[2], 0x000000);

        /* 3. Інструкція */
        DrawTextScaled(TerminusBold18x10_font, 20, 20, 
                       "🏹 GFX Arrow & Triangle Widget", spacing, 1, BLACK);
        DrawTextScaled(TerminusBold18x10_font, 20, 40, 
                       "Scanline fill + uint32_t colors + gfx_line", spacing, 1, DARKGRAY);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
