// main.c
#include "main.h"
#include "glyphs.h"
#include "cursor_gfx.h"
#include "all_font.h"

int spacing = 2;

int main(void) {
    const int W = 800, H = 450;
    gfx_open(W, H, "GFX Cursor Raster");
    gfx_enable_double_buffering(1);

    // Ініціалізуємо два курсори
    CursorGfx cursors[2];
    cursors[0] = InitCursor_GFX(200, 100, DEFAULT_CURSOR_WIDTH, DEFAULT_CURSOR_HEIGHT, RED, 0, 100);
    cursors[1] = InitCursor_GFX(600, 100, DEFAULT_CURSOR_WIDTH, DEFAULT_CURSOR_HEIGHT, BLUE, 0, 100);

    // Центральна ручка
    DragRectGfx centerRect = { 400, 150, 50, 20, GRAY, false };

    // Початковий фон
    DrawRectangleFast(0, 0, W, H, WHITE);

    gfx_enable_double_buffering(1);
    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        // ✅ ОБОВ'ЯЗКОВО: очищення буфера перед малюванням
        gfx_clear();
        // АБО заповнення фоном:
        // DrawRectangleFast(0, 0, W, H, WHITE);

        // Малюємо віджет
        DrawCursorsAndDistance_GFX(cursors, 2, TerminusBold24x12_font, &centerRect, W, H);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}



