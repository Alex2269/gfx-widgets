/* main.c */
#include "main.h"
#include "button_independent_gfx.h"
#include "all_font.h"
#include "glyphs.h"
#include <stdio.h>

int spacing = 2;
#define MAX_BUTTONS 4

// Базові кольори кнопок (0xRRGGBB)
static uint32_t button_colors[MAX_BUTTONS] = {
    0xFFFF00,  // YELLOW
    0x00FF00,  // GREEN
    0xFF0000,  // RED
    0x0000FF   // BLUE
};

int main(void) {
    const int W = 600, H = 200;
    gfx_open(W, H, "GFX Independent Buttons");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0x808080);  // GRAY фон

    // Стани кнопок: false = вимкнено, true = увімкнено
    bool button_states[MAX_BUTTONS] = { false, false, false, false };

    int posX = 50;
    int posY = 30;

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

        // Малюємо та обробляємо кожну кнопку окремо
        for (int i = 0; i < MAX_BUTTONS; i++) {
            int btnX = posX + i * 90;
            char label[8];
            snprintf(label, sizeof(label), "Btn %d", i + 1);

            // Клік змінює стан ЦІЄЇ кнопки незалежно від інших
            if (Gui_ButtonIndependent_GFX(btnX, posY, 80, 40,
                                          TerminusBold18x10_font, label, spacing,
                                          button_colors[i], 0,  // 0 = авто-контраст тексту
                                          button_states[i])) {
                button_states[i] = !button_states[i];  // Toggle
            }

            // Підпис під кнопкою: ON/OFF
            const char *status = button_states[i] ? "ON" : "OFF";
            int tw = utf8_strlen(status) * (TerminusBold18x10_font.glyph_width + spacing);
            int tx = btnX + (80 - tw) / 2;
            int ty = posY + 40 + 5;
            DrawTextScaled(TerminusBold18x10_font, tx, ty, status, spacing, 1, BLACK);
        }

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
