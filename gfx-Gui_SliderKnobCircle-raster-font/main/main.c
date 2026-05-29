/* main.c */
#include "main.h"
#include "slider_widget_circle_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 800, H = 450;
    gfx_open(W, H, "GFX Slider Knob Circle");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);

    float values[4] = { 0.0f, 2.25f, 5.5f, 3.0f };

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        gfx_clear();

        // Реєстрація та малювання слайдерів
        Gui_SliderKnobCircle_GFX(0, 100, 50, 10, 260, TerminusBold18x10_font, spacing,
                                 "tLeft1", NULL, &values[0], 250.0f, -250.0f, true, 0xFFFF00);
        Gui_SliderKnobCircle_GFX(1, 200, 50, 10, 260, TerminusBold18x10_font, spacing,
                                 "tLeft2", "tRight2", &values[1], -250.0f, 250.0f, true, 0x00FF00);
        Gui_SliderKnobCircle_GFX(2, 300, 50, 10, 260, TerminusBold18x10_font, spacing,
                                 "tLeft3", "tRight3", &values[2], 250.0f, -250.0f, true, 0xFF0000);
        Gui_SliderKnobCircle_GFX(3, 400, 50, 10, 260, TerminusBold18x10_font, spacing,
                                 "tLeft4", "tRight4", &values[3], -250.0f, 250.0f, true, 0x0000FF);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
