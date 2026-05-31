// main.c
#include "main.h"
#include "glyphs.h"
#include "gui_slider_spinner_gfx.h"
#include "all_font.h"
#include <stdio.h>

int spacing = 2;

// Значення для тесту
int iVal1 = 10;
int iVal2 = 10;
float fVal1 = 0.5f;
float fVal2 = 25.5f;
float fVal3 = 25.5f;
float fVal4 = 25.5f;

int intMin = -100, intMax = 100;
float floatMin = -100.0f, floatMax = 100.0f;
bool showButtons = true;

int main(void) {
    const int W = 600, H = 350;
    /* 1. Ініціалізація gfx */
    gfx_enable_double_buffering(1);
    gfx_open(W, H, "GFX SliderSpinner Raster");
    gfx_set_background_color(LIGHTGRAY);  // Світло сірий фон

    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        /* --- 4. ОЧИЩЕННЯ BACK-BUFFER --- */
        gfx_clear();  // Очищує back-buffer поточним фоновим кольором

        // Горизонтальний спінер (float, крок 2.5)
        Gui_SliderSpinner_GFX(0, 300, 100, 320, 40, "Менше", "Більше",
                              &fVal1, floatMax, floatMin,
                              2.5, GUI_SPINNER_GFX_FLOAT, GUI_SPINNER_GFX_HORIZONTAL,
                              YELLOW, TerminusBold24x12_font, spacing, showButtons);

        // Горизонтальний спінер (float, крок 2.5)
        Gui_SliderSpinner_GFX(1, 300, 250, 320, 40, "Менше", "Більше",
                              &fVal2, floatMin, floatMax,
                              2.5f, GUI_SPINNER_GFX_FLOAT, GUI_SPINNER_GFX_HORIZONTAL,
                              GREEN, TerminusBold24x12_font, spacing, showButtons);

        // Вертикальний спінер (int, крок 1)
        Gui_SliderSpinner_GFX(2, 50, 175, 20, 240, "Вгору", "Вниз",
                              &iVal1, intMax, intMin,
                              1, GUI_SPINNER_GFX_INT, GUI_SPINNER_GFX_VERTICAL,
                              BLUE, Terminus12x6_font, spacing, showButtons);

        // Вертикальний спінер (float, крок 2.5)
        Gui_SliderSpinner_GFX(3, 550, 175, 20, 240, "Вгору", "Вниз",
                              &fVal3, floatMin, floatMax,
                              2.5f, GUI_SPINNER_GFX_FLOAT, GUI_SPINNER_GFX_VERTICAL,
                              RED, Terminus12x6_font, spacing, showButtons);

        // Горизонтальний спінер (float, крок 2.5)
        Gui_SliderSpinner_GFX(4, 300, 300, 320, 20, "Менше", "Більше",
                              &fVal4, floatMin, floatMax,
                              2.5f, GUI_SPINNER_GFX_FLOAT, GUI_SPINNER_GFX_HORIZONTAL,
                              RED, Terminus12x6_font, spacing, !showButtons);

        // Відображення інформації вгорі
        char text1[64];
        char text2[64];
        char text3[64];
        snprintf(text1, sizeof(text1), "iVal1: %d", iVal1);
        snprintf(text2, sizeof(text2), "fVal1: %.2f\nfVal2: %.2f", fVal1, fVal2);
        snprintf(text3, sizeof(text3), "fVal3: %.2f\nfVal4: %.2f", fVal3, fVal4);

        DrawTextScaled(TerminusBold18x10_font, 30, 10, text1, spacing, 1, BLACK);
        DrawTextScaled(TerminusBold18x10_font, 120, 140, text2, spacing, 1, BLACK);
        DrawTextScaled(TerminusBold18x10_font, 350, 140, text3, spacing, 1, BLACK);

        /* --- 8. ПЕРЕМИКАННЯ БУФЕРІВ --- */
        gfx_swap_buffers();  // Показує back-buffer на екрані
        gfx_flush();         // Відправляє команди в X11

        // Невелика затримка для стабільності (~50 FPS)
        usleep(20);
    }
    return 0;
}
