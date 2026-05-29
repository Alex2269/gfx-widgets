/* main.c */
#include "main.h"
#include "cursor_vertical_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

/* Допоміжна функція для додавання курсора */
void AddNewCursor(CursorVerticalArrayGfx *arr, float fixedX, float startY, int screenHeight) {
    static uint32_t colors[] = { 0xFF0000, 0x0000FF, 0x00FF00, 0xFFA500, 0x800080 };
    static int colorIdx = 0;
    
    uint32_t col = colors[colorIdx % 5];
    colorIdx++;
    
    CursorVerticalGfx c = InitCursorVerticalGfx(fixedX, startY, 20, 10, col, 0, 100);
    /* Коригуємо початкове значення */
    float minY = c.height / 2;
    float maxY = screenHeight - c.height / 2;
    float norm = (startY - minY) / (maxY - minY);
    c.value = c.minValue + (int)((c.maxValue - c.minValue) * norm);
    
    AddCursorVerticalGfx(arr, c);
}

int main(void) {
    const int W = 800, H = 450;
    gfx_open(W, H, "GFX Vertical Cursors");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);

    /* Масив та стек */
    CursorVerticalArrayGfx cursors;
    ActiveCursorStackGfx activeStack;
    
    InitCursorVerticalArrayGfx(&cursors, 4);
    InitActiveCursorStackGfx(&activeStack);

    float fixedX = W - 50;

    /* Додаємо 2 початкові ручки */
    AddNewCursor(&cursors, fixedX, H / 3.0f, H);
    AddNewCursor(&cursors, fixedX, H / 1.5f, H);

    /* Початкове очищення */
    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
            
            /* Додавання/видалення клавішами */
            if(k == '+' || k == '=') AddNewCursor(&cursors, fixedX, H/2.0f, H);
            if((k == '-' || k == '_') && cursors.count > 0) {
                RemoveCursorVerticalGfx(&cursors, cursors.count - 1);
            }
        }

        /* Очищення кадру */
        gfx_clear();

        /* Оновлення логіки */
        UpdateAndHandleCursorVerticalsGfx(&cursors, H,
            gfx_is_mouse_pressed(GFX_MOUSE_LEFT),
            gfx_is_mouse_down(GFX_MOUSE_LEFT),
            gfx_is_mouse_released(GFX_MOUSE_LEFT),
            &activeStack);

        /* Малювання */
        /* Фон області */
        gfx_draw_fast_rect(10, 10, W-20, H-20, 0xE0E0E0);
        
        DrawCursorVerticalsGfx(&cursors, TerminusBold18x10_font, spacing);

        /* Інструкція */
        DrawTextScaled(TerminusBold18x10_font, 20, 20, 
                       "ЛКМ: перетягування | +/-: додати/видалити | Esc: вихід",
                       spacing, 1, BLACK);

        gfx_swap_buffers();
        usleep(10000);
    }

    FreeCursorVerticalArrayGfx(&cursors);
    return 0;
}
