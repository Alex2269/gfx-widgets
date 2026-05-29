// main.c
#include "main.h"
#include "glyphs.h"
#include "gui_slider_spinner_gfx.h"
#include "all_font.h"

int spacing = 2;

// Значення для тесту
int iVal1 = 10;
float fVal1 = 25.5f, fVal2 = 25.5f, fVal3 = 25.5f, fVal4 = 25.5f;
int intMin = -100, intMax = 100;
float floatMin = -100.0f, floatMax = 100.0f;
bool showButtons = true;

int main(void) {
    const int W = 800, H = 450;
    gfx_open(W, H, "GFX SliderSpinner Raster");
    gfx_enable_double_buffering(1);

    // Початковий фон
    DrawRectangleFast(0, 0, W, H, WHITE);
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        // Горизонтальний спінер (float)
        Gui_SliderSpinner_GFX(0, 400, 100, 320, 40, "Менше", "Більше",
                    &fVal1, &floatMax, &floatMin, 0.5f, GUI_SPINNER_GFX_FLOAT,
                    GUI_SPINNER_GFX_HORIZONTAL, YELLOW, TerminusBold24x12_font, spacing, showButtons);

        // Вертикальний спінер (int) - "Вгору" зверху, "Вниз" знизу
        Gui_SliderSpinner_GFX(1, 100, 250, 40, 240, "Вгору", "Вниз",
                    &iVal1, &intMax, &intMin, 1.0f, GUI_SPINNER_GFX_INT,
                    GUI_SPINNER_GFX_VERTICAL, BLUE, Terminus12x6_font, spacing, showButtons);

        // Вертикальний спінер (float)
        Gui_SliderSpinner_GFX(2, 700, 250, 40, 240, "Вгору", "Вниз",
                    &fVal2, &floatMin, &floatMax, 0.5f, GUI_SPINNER_GFX_FLOAT,
                    GUI_SPINNER_GFX_VERTICAL, RED, Terminus12x6_font, spacing, showButtons);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
