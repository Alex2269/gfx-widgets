// main.c
#include "main.h"
#include "button_extended_gfx.h"
#include "all_font.h"
#include "glyphs.h"
#include <unistd.h>

int spacing = 2;

int main(void) {
    const int W = 600, H = 300;
    gfx_open(W, H, "GFX Button Extended Example");
    gfx_enable_double_buffering(1);
    
    // Стан кнопки
    ButtonStateGfx btnState = {0};
    
    // Прапорці подій
    bool isLongPress = false;
    bool isDoubleClick = false;
    bool isSingleClick = false;
    
    // Таймер для відображення повідомлення
    double msgDisplayUntil = 0.0;
    const double msgDuration = 1.5;  // 1.5 секунди
    const char* msgText = "";
    
    // Початковий фон
    DrawRectangleFast(0, 0, W, H, WHITE);
    gfx_swap_buffers();
    
    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }
        
        double currentTime = GetTimeGfx();  // Потрібно додати цю функцію
        
        // Очищення екрану
        DrawRectangleFast(0, 0, W, H, WHITE);
        
        // Кнопка: x=100, y=100, width=150, height=50
        isSingleClick = Gui_ButtonExtended_GFX(100, 100, 150, 50,
                                               TerminusBold24x12_font, "Press Me", spacing,
                                               0x87CEEB,        // SKYBLUE = 0x87CEEB
                                               0x70B8E0,        // Темніший SKYBLUE
                                               0x4682B4,        // STEELBLUE
                                               0x000080,        // DARKBLUE для тексту
                                               &btnState,
                                               &isLongPress, &isDoubleClick);
        
        // Визначаємо повідомлення
        if (isLongPress) {
            msgText = "Long Press";
            msgDisplayUntil = currentTime + msgDuration;
        }
        else if (isDoubleClick) {
            msgText = "Double Click";
            msgDisplayUntil = currentTime + msgDuration;
        }
        else if (isSingleClick) {
            msgText = "Single Click";
            msgDisplayUntil = currentTime + msgDuration;
        }
        
        // Відображаємо повідомлення, якщо таймер не минув
        if (currentTime < msgDisplayUntil) {
            int textWidth = utf8_strlen(msgText) * (TerminusBold24x12_font.glyph_width + spacing);
            int textX = 300 - textWidth / 2;
            DrawTextScaled(TerminusBold24x12_font, textX, 100, msgText, spacing, 1, BLACK);
        }
        
        gfx_swap_buffers();
        usleep(10000);
    }
    
    return 0;
}
