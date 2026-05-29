/* main.c */
#include "main.h"
#include "cam_switch_log_gfx.h"
#include "all_font.h"
#include "glyphs.h"

int spacing = 2;

int main(void) {
    const int W = 900, H = 450;
    gfx_open(W, H, "GFX CamSwitch Log Example");
    gfx_enable_double_buffering(1);
    gfx_set_background_color(0xF0F0F0);

    /* Значення: одне логарифмічне, одне лінійне */
    float valLog = 100.0f;   /* Логарифмічний: 2..5000 */
    float valLin = 0.0f;     /* Лінійний: -100..100 */

    gfx_clear();
    gfx_swap_buffers();

    while(1) {
        gfx_process_events();
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        gfx_clear();

        /* 🔹 Логарифмічний перемикач (шкала: 2, 5, 10... 5000) */
        Gui_CamSwitch_Log_GFX(0, 250, 200, 60, &valLog, 0, 0,  /* min/max ігноруються */
                              true,  /* ✅ logarithmic = true */
                              0xFF0000,  /* RED */
                              "Масштаб", "CH1",
                              Terminus12x6_font, spacing);

        /* 🔹 Лінійний перемикач (шкала: -100..100) */
        Gui_CamSwitch_Log_GFX(1, 500, 200, 60, &valLin, -100.0f, 100.0f,
                              false,  /* ✅ logarithmic = false */
                              0x0000FF,  /* BLUE */
                              "Зсув", "CH2",
                              Terminus12x6_font, spacing);

        /* Відображення поточних значень */
        char logBuf[32], linBuf[32];
        snprintf(logBuf, sizeof(logBuf), "Log: %.0f", valLog);
        snprintf(linBuf, sizeof(linBuf), "Lin: %.1f", valLin);
        
        DrawTextScaled(Terminus12x6_font, 250 - 30, 280, logBuf, spacing, 1, 0xFF0000);
        DrawTextScaled(Terminus12x6_font, 500 - 30, 280, linBuf, spacing, 1, 0x0000FF);

        gfx_swap_buffers();
        usleep(10000);
    }
    return 0;
}
