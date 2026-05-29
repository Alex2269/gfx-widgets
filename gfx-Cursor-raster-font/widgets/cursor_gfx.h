/*
cursor_gfx.h
Віджет курсорів для X11/gfx з підтримкою миші (raylib-style polling)
Використовує RasterFont та uint32_t кольори (0xRRGGBB)
*/
#ifndef CURSOR_GFX_H
#define CURSOR_GFX_H
#include <stdint.h>
#include <stdbool.h>
#include "glyphs.h"
#include "graphics.h"
#ifdef __cplusplus
extern "C" {
#endif

/* ================= КОНСТАНТИ ЗА ЗАМОВЧУВАННЯМ ================= */
#define DEFAULT_CURSOR_WIDTH   10.0f
#define DEFAULT_CURSOR_HEIGHT  20.0f
#define DEFAULT_CURSOR_TOP_Y   5
#define ARROW_SIZE             10
#define THIN_LINE_HEIGHT       100

/* ================= СТРУКТУРИ ================= */
/** @brief Курсор — рухомий маркер по горизонталі */
typedef struct {
    float x;           // Позиція курсора по горизонталі (центр прямокутника)
    float y;           // Позиція для відображення тексту і лінії під курсором
    float topY;        // Верхня позиція курсора по вертикалі
    float width;       // Ширина прямокутника курсора
    float height;      // Висота прямокутника курсора
    uint32_t color;    // Колір курсора (0xRRGGBB)
    bool isDragging;   // Чи перетягується курсор зараз
    int value;         // Значення курсора (пропорційне позиції по X)
    int minValue;      // Мінімальне значення курсора
    int maxValue;      // Максимальне значення курсора
    int min_X;         // Мінімальна позиція курсора по X
    int max_X;         // Максимальна позиція курсора по X
} CursorGfx;

/** @brief Центральний прямокутник (ручка) на горизонтальній лінії між курсорами */
typedef struct {
    float x;           // Позиція по горизонталі (центр)
    float y;           // Позиція по вертикалі (центр)
    float width;       // Ширина прямокутника
    float height;      // Висота прямокутника
    uint32_t color;    // Колір прямокутника (0xRRGGBB)
    bool isDragging;   // Чи перетягується прямокутник зараз
} DragRectGfx;

/* ================= ІНІЦІАЛІЗАЦІЯ ================= */
CursorGfx InitCursor_GFX(float startX, float topY, float width, float height,
                         uint32_t color, int minValue, int maxValue);

/* ================= ОНОВЛЕННЯ ТА ОБРОБКА ВВОДУ ================= */
void UpdateAndHandleCursors_GFX(CursorGfx *cursors, int count,
                                int mouseX, int mouseY,
                                bool mousePressed, bool mouseDown, bool mouseReleased);

void UpdateAndHandleCenterRect_GFX(DragRectGfx *rect,
                                   int mouseX, int mouseY,
                                   bool mousePressed, bool mouseDown, bool mouseReleased,
                                   int screenHeight);  /* ✅ ДОДАНО ПАРАМЕТР */

/* ================= МАЛЮВАННЯ ================= */
void DrawCursorsAndDistance_GFX(CursorGfx *cursors, int count,
                                RasterFont font, DragRectGfx *centerRect,
                                int screenWidth, int screenHeight);

#ifdef __cplusplus
}
#endif
#endif /* CURSOR_GFX_H */
