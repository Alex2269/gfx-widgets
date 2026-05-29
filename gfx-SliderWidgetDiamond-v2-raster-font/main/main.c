/* main.c */
#include "main.h"
#include "slider_knob_diamond_gfx.h"
#include "all_font.h"
#include "glyphs.h"
#include <stdio.h>

int spacing = 2;

int main(void) {
    const int W = 800, H = 450;
    gfx_open(W, H, "GFX Diamond Slider 3D");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);

    float values[4] = { 0.0f, 50.0f, -100.0f, 150.0f };
    uint32_t colors[4] = { 0xFFFF00, 0x00FF00, 0xFF0000, 0x87CEEB }; // YELLOW, GREEN, RED, SKYBLUE
    const char* tooltips_top[4] = { "Offset\nCH1", "Offset\nCH2", "Offset\nCH3", "Offset\nCH4" };

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        gfx_clear();

        // Заголовок
        DrawTextScaled(TerminusBold24x12_font, 20, 15, "💎 DiamondKnob Sliders 3D Demo", spacing, 1, BLACK);
        DrawTextScaled(TerminusBold18x10_font, 20, 40, "Drag the diamond knobs to adjust values", spacing, 1, DARKGRAY);

        // Реєстрація та малювання слайдерів
        int startX = 100, startY = 75, spacingX = 125;
        for (int i = 0; i < 4; i++) {
            Gui_SliderKnobDiamond_GFX(i, startX + i * spacingX, startY, 10, 260, 
                                      TerminusBold18x10_font, spacing,
                                      tooltips_top[i], NULL, 
                                      &values[i], -250.0f, 250.0f, true, colors[i]);
        }

        // Відображення поточних значень знизу (як у v2)
        int infoY = startY + 260 + 30;
        for (int i = 0; i < 4; i++) {
            char valText[32];
            snprintf(valText, sizeof(valText), "CH%d: %3.1f", i + 1, values[i]);
            
            // Підсвітка тексту при наведенні на слайдер
            uint32_t txtColor = colors[i];
            // Проста перевірка наведення для підсвітки тексту (опціонально)
            // ... (можна додати логіку наведення тут, якщо потрібно)
            
            DrawTextScaled(TerminusBold18x10_font, startX + i * spacingX, infoY, valText, spacing, 1, txtColor);
        }

        // FPS та інфо
        char fpsText[64];
        snprintf(fpsText, sizeof(fpsText), "FPS: %d | Values: [%.1f, %.1f, %.1f, %.1f]", 
                 60, values[0], values[1], values[2], values[3]); // Приблизна оцінка FPS
        DrawTextScaled(TerminusBold18x10_font, 10, H - 20, fpsText, spacing, 1, DARKGRAY);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
