/* main.c */
#include "main.h"
#include "cam_switch_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 900, H = 450;
    gfx_open(W, H, "GFX CamSwitch Example");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);

    /* Значення: одне зі звичайним діапазоном, одне інвертоване */
    float val1 = 0.0f;    /* Діапазон: -2..2 */
    float val2 = 5.0f;    /* Діапазон: 20..-20 (інверсія) */

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        gfx_clear();

        /* 🔹 Звичайний діапазон (-2..2) */
        Gui_CamSwitch_GFX(0, 300, 200, 60, &val1, -2.0f, 2.0f,
                          0xFF0000,  /* RED */
                          "Масштаб", "CH1",
                          TerminusBold18x10_font, spacing);

        /* 🔹 Інвертований діапазон (20..-20) */
        Gui_CamSwitch_GFX(1, 600, 200, 60, &val2, 20.0f, -20.0f,
                          0x0000FF,  /* BLUE */
                          "Зсув", "CH2",
                          TerminusBold18x10_font, spacing);

        /* Відображення поточних значень */
        char buf1[32], buf2[32];
        snprintf(buf1, sizeof(buf1), "CH1: %.2f", val1);
        snprintf(buf2, sizeof(buf2), "CH2: %.2f", val2);
        
        DrawTextScaled(TerminusBold18x10_font, 300 - 30, 280, buf1, spacing, 1, 0xFF0000);
        DrawTextScaled(TerminusBold18x10_font, 600 - 30, 280, buf2, spacing, 1, 0x0000FF);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
