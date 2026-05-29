/* main.c */
#include "main.h"
#include "arrow_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 600, H = 450;
    gfx_open(W, H, "GFX Arrow & Triangle Demo");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0x000000); /* Чорний фон */

    float angle = 0.0f;
    double last_time = GetTimeGfx();

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        /* Анімація обертання */
        double now = GetTimeGfx();
        angle += (now - last_time) * 90.0f; /* 90 градусів за секунду */
        if (angle > 360.0f) angle -= 360.0f;
        last_time = now;

        gfx_clear();

        /* 1. Стрілки під різними кутами */
        gfx_draw_arrow(100, 100, 0.0f * DEG2RAD, 40.0f, YELLOW);       /* Праворуч */
        gfx_draw_arrow(100, 150, 90.0f * DEG2RAD, 40.0f, GREEN);       /* Вниз */
        gfx_draw_arrow(100, 200, 180.0f * DEG2RAD, 40.0f, RED);        /* Ліворуч */
        gfx_draw_arrow(100, 250, 270.0f * DEG2RAD, 40.0f, BLUE);       /* Вгору */

        /* 2. Обертаний рівносторонній трикутник (заповнений) */
        gfx_draw_filled_triangle_rotated(350, 175, 60.0f, angle, 0x87CEEB); /* SKYBLUE */
        
        /* Контур навколо нього */
        gfx_draw_triangle_lines_rotated(350, 175, 65.0f, angle, WHITE);

        /* 3. Інструкція */
        DrawTextScaled(TerminusBold24x12_font, 20, 20, 
                       "🏹 GFX Arrow & Triangle Widget", spacing, 1, WHITE);
        DrawTextScaled(TerminusBold18x10_font, 20, 50, 
                       "Scanline fill + uint32_t colors + gfx_line", spacing, 1, 0xAAAAAA);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
