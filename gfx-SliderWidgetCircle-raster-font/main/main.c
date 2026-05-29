/* main.c */
#include "main.h"
#include "slider_widget_circle_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 800, H = 450;
    gfx_open(W, H, "GFX Circle Knob Sliders");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0x808080);  // GRAY фон

    // Значення слайдерів
    float values[4] = { 0.0f, 2.25f, 5.5f, 3.0f };

    // Початкове очищення
    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        // Очищення кадру
        gfx_clear();

        // Реєстрація 4 вертикальних слайдерів
        RegisterCircleKnobSliderGfx(0, 100, 50, 10, 260, &values[0], 250.0f, -250.0f, true, 0xFFFF00, "tLeft", "tRight");
        RegisterCircleKnobSliderGfx(1, 200, 50, 10, 260, &values[1], -250.0f, 250.0f, true, 0x00FF00, "tLeft", "tRight");
        RegisterCircleKnobSliderGfx(2, 300, 50, 10, 260, &values[2], 250.0f, -250.0f, true, 0xFF0000, "tLeft", "tRight");
        RegisterCircleKnobSliderGfx(3, 400, 50, 10, 260, &values[3], -250.0f, 250.0f, true, 0x0000FF, "tLeft", "tRight");

        // Оновлення та малювання всіх слайдерів
        UpdateCircleKnobSlidersAndDrawGfx(TerminusBold18x10_font, spacing);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
