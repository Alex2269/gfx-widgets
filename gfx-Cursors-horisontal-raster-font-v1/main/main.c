/* main.c */
#include "main.h"
#include "cursor.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 800, H = 600;
    gfx_open(W, H, "GFX Cursors Example");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0x404040); /* DARKGRAY */

    /* Ініціалізація двох курсорів */
    Cursor cursors[2];
    cursors[0] = InitCursor(225.0f, DEFAULT_CURSOR_TOP_Y, DEFAULT_CURSOR_WIDTH, DEFAULT_CURSOR_HEIGHT,
                               0xFF0000, 0, 100); /* RED */
    cursors[1] = InitCursor(425.0f, DEFAULT_CURSOR_TOP_Y, DEFAULT_CURSOR_WIDTH, DEFAULT_CURSOR_HEIGHT,
                               0x0000FF, 0, 100); /* BLUE */

    /* Центральна ручка */
    DragRect centerRect = {
        .x = (cursors[0].x + cursors[1].x) / 2.0f,
        .y = cursors[0].y,
        .width = 20,
        .height = 10,
        .color = 0xC0C0C0, /* LIGHTGRAY */
        .isDragging = false
    };

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

        /* Фон осцилоскопа */
        DrawRectangleFast(10, 10, W - 20, H - 20, 0x202020);

        /* Малювання курсорів */
        DrawCursorsAndDistance(cursors, 2, TerminusBold18x10_font, &centerRect, W, H);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
