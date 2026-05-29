/* main.c */
#include "main.h"
#include "gui_radiobutton_row_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 800, H = 450;
    gfx_open(W, H, "GFX Radio Buttons Row");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);  /* Світло-сірий фон */

    /* Дані для радіокнопок */
    const char *radioItems[] = { "R", "F", "A" };
    int itemCount = 3;
    int selectedIndex = 0;  /* Початковий вибір */

    /* Параметри віджета */
    int radioX = 100, radioY = 75;
    int buttonSize = 30;
    int btnSpacing = 5;
    uint32_t activeColor = 0xFFFF00;  /* YELLOW */

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

        /* Віджет радіокнопок — повертає новий індекс вибору */
        selectedIndex = Gui_RadioButtons_Row_GFX(radioX, radioY,
                                                  radioItems, itemCount,
                                                  selectedIndex, activeColor,
                                                  buttonSize, btnSpacing,
                                                  TerminusBold24x12_font, spacing);

        /* Відображення поточного вибору */
        char status[64];
        snprintf(status, sizeof(status), "Вибрано: %s", radioItems[selectedIndex]);
        DrawTextScaled(TerminusBold24x12_font, 50, 20, status, spacing, 2, BLACK);

        /* Додаткова інформація */
        DrawTextScaled(TerminusBold18x10_font, 50, 60,
                       "Натисніть на кнопку, щоб змінити вибір",
                       spacing, 1, DARKGRAY);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
