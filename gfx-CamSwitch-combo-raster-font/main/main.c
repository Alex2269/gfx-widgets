// main.c
#include "main.h"
#include "glyphs.h"
#include "cam_switch_gfx.h"
#include "all_font.h"

// Глобальна змінна spacing для узгодження з віджетами
int spacing = 2;

int main(void) {
    const int W = 800;
    const int H = 450;

    // Ініціалізація вікна gfx
    gfx_open(W, H, "GFX CamSwitch Raster");

    // ✅ УВІМКНУТИ ПОДВІЙНИЙ БУФЕР (усуває мерехтіння)
    gfx_enable_double_buffering(1);

    // Значення регуляторів
    float val_log = 10.0f;  // Логарифмічний канал
    float val_lin = 2.5f;   // Лінійний канал
    float radius = 44.0f;   // Радіус ручки

    // Початкове малювання фону (тільки один раз перед циклом)
    DrawRectangleFast(0, 0, W, H, WHITE);
    gfx_swap_buffers();

    // Головний цикл програми
    while(1) {
        // 1. Оновлюємо стан миші/подій (неблокуюче)
        gfx_process_events();

        // 2. Перевірка на вихід (ESC або 'q')
        if(gfx_event_waiting()) {
            char k = gfx_wait();
            if(k == 27 || k == 'q') break;
        }

        // ✅ Фон НЕ перемальовуємо — він залишається з попереднього кадру
        // Малюємо ТІЛЬКИ змінені області (віджети)

        // 🎛 Логарифмічний канал (шкала: 2, 5, 10... 5000)
        Gui_CamSwitch_GFX(0,
                          Terminus12x6_font,      // шрифт підписів шкали
                          TerminusBold18x10_font, // шрифт значення
                          200, 200,               // позиція центру
                          "Масштабування 1",      // tooltip при наведенні
                          NULL,                   // текст праворуч
                          radius * 2,             // радіус ручки
                          &val_log,               // вказівник на значення
                          2.0f, 5000.0f,          // діапазон
                          true,                   // активний
                          YELLOW,                 // колір тексту/стрілки
                          true);                  // ✅ логарифмічна шкала

        // 🎛 Лінійний канал (шкала: 0.0...5.0)
        Gui_CamSwitch_GFX(1,
                          Terminus12x6_font,
                          TerminusBold18x10_font,
                          500, 200,
                          "Масштабування 2",
                          NULL,
                          radius,
                          &val_lin,
                          0.0f, 5.0f,
                          true,
                          WHITE,
                          false);                 // ✅ лінійна шкала

        // ✅ Перенести буфер на екран одним блітом (усуває мерехтіння)
        gfx_swap_buffers();

        // Обмеження кадрів ~100 FPS
        usleep(10000);
    }

    return 0;
}
