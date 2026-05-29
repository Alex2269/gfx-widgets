/* main/main.c */
#include "main.h"
#include "button_selector_gfx.h"
#include "all_font.h"
#include "glyphs.h"
#include <stdio.h>

int spacing = 2;
#define MAX_CHANNELS 4

// Кольори каналів (формат 0xRRGGBB)
static uint32_t channel_colors[MAX_CHANNELS] = {
    0xFFFF00, // YELLOW
    0x00FF00, // GREEN
    0xFF0000, // RED
    0x0000FF  // BLUE
};

int main(void) {
    const int W = 600, H = 200;
    gfx_open(W, H, "GFX Channel Selector");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0x808080); // GRAY

    int posX = 50;
    int posY = 30;
    int active_button = 0;

    // Початкове малювання фону
    DrawRectangleFast(0, 0, W, H, 0x808080);
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        // Очищення кадру
        DrawRectangleFast(0, 0, W, H, 0x808080);

        // Малювання кнопок вибору каналу
        for (int i = 0; i < MAX_CHANNELS; i++) {
            int btnX = posX + i * 80;
            uint32_t baseColor = channel_colors[i];
            
            // ✅ Адаптація Fade: імітація затемнення неактивних кнопок
            uint32_t btnNormal = (active_button == i) ? baseColor : 0x444444;
            uint32_t btnHover  = (active_button == i) ? baseColor : 0x666666;

            char label[8];
            snprintf(label, sizeof(label), "BT%d", i + 1);

            if (Gui_ButtonSelector_GFX(btnX, posY, 70, 40, TerminusBold18x10_font,
                                       label, spacing, btnNormal, btnHover, baseColor, 0, active_button == i)) {
                active_button = i;
            }
        }

        // Вивід стану
        char status[64];
        snprintf(status, sizeof(status), "Кнопка %d ВКЛ", active_button + 1);
        DrawTextScaled(TerminusBold24x12_font, 50, 100, status, spacing, 2, BLACK);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
