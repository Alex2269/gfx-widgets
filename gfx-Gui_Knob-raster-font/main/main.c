/* main.c */
#include "main.h"
#include "knob_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 800, H = 450;
    gfx_open(W, H, "GFX Knob Example");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);

    float val1 = 0.0f, val2 = 50.0f;

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        gfx_clear();

        // Регулятор 1: Діапазон -100..100
        Gui_Knob_GFX(0, 200, 200, 60, &val1, -100.0f, 100.0f, 0xFF0000, "Volume", TerminusBold18x10_font);

        // Регулятор 2: Інвертований діапазон (100..0)
        Gui_Knob_GFX(1, 400, 200, 60, &val2, 100.0f, 0.0f, 0x0000FF, "Brightness", TerminusBold18x10_font);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
