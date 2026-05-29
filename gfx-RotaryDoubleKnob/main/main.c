/* main.c */
#include "main.h"
#include "dual_knob_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 800, H = 450;
    gfx_open(W, H, "GFX Dual Vernier Knob");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);

    /* Ініціалізація регулятора */
    DualKnobGfx knob = InitDualKnobGfx(W / 2, H / 2, 70.0f);

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        gfx_clear();

        /* Малювання та оновлення (активний) */
        UpdateAndDrawDualKnobGfx(&knob, TerminusBold24x12_font, true);

        /* Інструкція */
        DrawTextScaled(TerminusBold24x12_font, 20, 20, 
                       "ЛКМ: тягни кільця. Зовнішнє - повільно, Внутрішнє - швидко", 
                       spacing, 1, BLACK);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
