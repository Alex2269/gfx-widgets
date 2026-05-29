/* main.c */
#include "main.h"
#include "draw_grid_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 1000, H = 600;
    gfx_open(W, H, "GFX DrawGrid Example");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0x000000); // Чорний фон

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break; // ESC або Q для виходу
        }

        // Очищення кадру
        gfx_clear();

        // Малюємо сітку
        // cellSize = 50, padding = 49 (для центрування)
        draw_grid_gfx(W, H, 50, 49);

        // Заголовок
        DrawTextScaled(TerminusBold24x12_font, 20, 20, 
                       "💠 GFX Oscilloscope Grid", spacing, 1, WHITE);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
