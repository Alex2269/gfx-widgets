// main.c
#include "main.h"
#include "button_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 600, H = 200;
    gfx_open(W, H, "GFX Button Example");
    gfx_enable_double_buffering(1);

    bool btn1State = false;
    bool btn2State = false;

    // Початковий фон
    DrawRectangleFast(0, 0, W, H, LIGHTGRAY);
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        // Очищення кадру
        DrawRectangleFast(0, 0, W, H, LIGHTGRAY);

        // Кнопка 1 (Toggle)
        if (Gui_Button_GFX(50, 50, 120, 40, TerminusBold24x12_font, "Кнопка 1", spacing,
                           0x87CEEB, 0x70B8E0, 0x4682B4, 0))
            btn1State = !btn1State;

        // Кнопка 2 (Toggle)
        if (Gui_Button_GFX(200, 50, 120, 40, TerminusBold24x12_font, "Кнопка 2", spacing,
                           0xD3D3D3, 0xB0B0B0, 0x696969, 0))
            btn2State = !btn2State;

        // Кнопка "Вийти"
        if (Gui_Button_GFX(350, 50, 120, 40, TerminusBold24x12_font, "Вийти", spacing,
                           0xFFC0CB, 0xE0A0B0, 0x800000, 0xFFFFFF))
            break;

        // Індикація станів
        DrawTextScaled(TerminusBold24x12_font, 50, 110,
                       btn1State ? "Кнопка\n1 ВКЛ" : "Кнопка\n1 ВИКЛ", spacing, 1, BLACK);
        DrawTextScaled(TerminusBold24x12_font, 200, 110,
                       btn2State ? "Кнопка\n2 ВКЛ" : "Кнопка\n2 ВИКЛ", spacing, 1, BLACK);
        DrawTextScaled(TerminusBold24x12_font, 350, 110,
                       "Клікни Вийти\nщоб закрити", spacing, 1, BLACK);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
