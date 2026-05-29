/* main.c */
#include "main.h"
#include "gui_checkbox_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 600, H = 300;
    gfx_open(W, H, "GFX CheckBox Example");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);  /* Світло-сірий фон */

    /* Стани чекбоксів */
    bool opt1 = false, opt2 = true, opt3 = false;

    /* Початкове очищення */
    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        /* Очищення кадру */
        gfx_clear();

        /* Чекбокси вертикально */
        Gui_CheckBox_GFX(50, 50, 30, &opt1, TerminusBold24x12_font,
                         "Підказка 1", "Варіант 1", 0x00FF00, spacing);  /* GREEN */

        Gui_CheckBox_GFX(50, 100, 30, &opt2, TerminusBold24x12_font,
                         "Підказка 2", "Варіант 2", 0xFFFF00, spacing);  /* YELLOW */

        Gui_CheckBox_GFX(50, 150, 30, &opt3, TerminusBold24x12_font,
                         "Підказка 3", "Варіант 3", 0xFF0000, spacing);  /* RED */

        /* Відображення вибору */
        char status[128] = "Вибрано: ";
        if (opt1) strcat(status, "1 ");
        if (opt2) strcat(status, "2 ");
        if (opt3) strcat(status, "3 ");
        if (!opt1 && !opt2 && !opt3) strcat(status, "нічого");

        DrawTextScaled(TerminusBold24x12_font, 50, 220, status, spacing, 2, BLACK);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
