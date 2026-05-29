// cursor.h
#ifndef CURSOR_H
#define CURSOR_H

#include <stdint.h>
#include <stdbool.h>
#include "glyphs.h"
#include "graphics.h"

// Константи за замовчуванням для курсорів і графіки
#define DEFAULT_CURSOR_WIDTH 10.0f
#define DEFAULT_CURSOR_HEIGHT 20.0f
#define DEFAULT_CURSOR_TOP_Y 5
#define ARROW_SIZE 10
#define THIN_LINE 100

/*
 * Структура для курсора (рухомий маркер по горизонталі)
 * ✅ Color замінено на uint32_t для сумісності з gfx
 */
typedef struct {
    float x;          // Позиція курсора по горизонталі (центр прямокутника)
    float y;          // Позиція для відображення тексту і лінії під курсором (динамічна)
    float topY;       // Верхня позиція курсора по вертикалі (верхній край прямокутника)
    float width;      // Ширина прямокутника курсора
    float height;     // Висота прямокутника курсора
    uint32_t color;   // ✅ Колір курсора (0xRRGGBB) для gfx
    bool isDragging;  // Чи перетягується курсор зараз
    int value;        // Значення курсора (пропорційне позиції по X)
    int minValue;     // Мінімальне значення курсора
    int maxValue;     // Максимальне значення курсора

    int min_X;        // Мінімальна позиція курсора (початкове обмеження)
    int max_X;        // Максимальна позиція курсора (обмеження по X)

} Cursor;

/*
 * Структура для центрального прямокутника (ручки)
 */
typedef struct {
    float x;          // Позиція по горизонталі (центр прямокутника)
    float y;          // Позиція по вертикалі (центр прямокутника)
    float width;      // Ширина прямокутника
    float height;     // Висота прямокутника
    uint32_t color;   // ✅ Колір прямокутника (0xRRGGBB) для gfx
    bool isDragging;  // Чи перетягується прямокутник зараз
} DragRect;

/*
 * Ініціалізує курсор з заданими параметрами.
 */
Cursor InitCursor(float startX, float topY, float width, float height, 
                  uint32_t color, int minValue, int maxValue);

/*
 * Оновлює позиції і обробляє взаємодію з усіма курсорами.
 * ✅ mousePos тепер Vector2, але внутрішньо використовується gfx_polling
 */
void UpdateAndHandleCursors(Cursor *cursors, int count, Vector2 mousePos, 
                            bool mouseButtonPressed, bool mouseButtonDown, 
                            bool mouseButtonReleased);

/*
 * Малює курсори, лінії між ними, стрілки, центральний прямокутник та відстань.
 * ✅ Додано screenWidth/screenHeight для обмежень у gfx
 */
void DrawCursorsAndDistance(Cursor *cursors, int count, RasterFont font, 
                            DragRect *centerRect, 
                            int screenWidth, int screenHeight);

/*
 * Перевіряє, чи знаходиться курсор миші над прямокутником.
 */
bool IsMouseOverRect(Vector2 mousePos, DragRect rect);

/*
 * Оновлює позицію центрального прямокутника (ручки) при перетягуванні мишею.
 */
void UpdateAndHandleCenterRect(DragRect *rect, Vector2 mousePos,
                               bool mouseButtonPressed, bool mouseButtonDown,
                               bool mouseButtonReleased,
                               int screenHeight);

#endif // CURSOR_H
