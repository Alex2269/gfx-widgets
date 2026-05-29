/* main.c */
#include "main.h"
#include "draw_hscale_gfx.h"
#include "draw_vscale_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 600, H = 600;
    gfx_open(W, H, "GFX Scale Example");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);

    float scale = 2.0f;
    float offset_y = 50.0f;
    float offset_x = 50.0f;

    // Область для вертикальної шкали
    int v_x = 20, v_y = 20;
    int v_w = 60, v_h = H - 40;

    // Область для горизонтальної шкали
    int h_x = v_x + v_w + 20;
    int h_y = H - 60;
    int h_w = W - v_x - v_w - 40;
    int h_h = 50;

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        // Анімація зміщення
        offset_y += 0.05f; if (offset_y > 150.0f) offset_y = 0.0f;
        offset_x += 0.05f; if (offset_x > 150.0f) offset_x = 0.0f;

        gfx_clear();

        // Малюємо шкали
        DrawVerticalScale_GFX(0, scale, offset_y, v_x, v_y, v_w, v_h, TerminusBold24x12_font, BLACK);
        DrawHorizontalScale_GFX(0, scale, offset_x, h_x, h_y, h_w, h_h, TerminusBold24x12_font, BLACK);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
