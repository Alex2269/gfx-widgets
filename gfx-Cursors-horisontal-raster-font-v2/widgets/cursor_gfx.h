/* cursor_gfx.h */
#ifndef CURSOR_GFX_H
#define CURSOR_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdbool.h>
#include <stdint.h>

/* Константи за замовчуванням */
#define DEFAULT_CURSOR_WIDTH 10.0f
#define DEFAULT_CURSOR_HEIGHT 20.0f
#define DEFAULT_CURSOR_TOP_Y 5
#define ARROW_SIZE_GFX 10
#define THIN_LINE_GFX 100

/**
 * @brief Структура курсора (рухомий маркер по горизонталі)
 */
typedef struct {
    float x;          /* Позиція по X (центр прямокутника) */
    float y;          /* Позиція для тексту/лінії (динамічна) */
    float topY;       /* Верхня позиція курсора (верхній край) */
    float width;      /* Ширина прямокутника */
    float height;     /* Висота прямокутника */
    uint32_t color;   /* Колір курсора (0xRRGGBB) */
    bool isDragging;  /* Чи перетягується зараз */
    int value;        /* Значення курсора (пропорційне X) */
    int minValue;     /* Мін. значення */
    int maxValue;     /* Макс. значення */
    int min_X;        /* Мін. позиція X (обмеження) */
    int max_X;        /* Макс. позиція X (обмеження) */
} CursorGfx;

/**
 * @brief Структура центрального прямокутника (ручки) на лінії
 */
typedef struct {
    float x;          /* Позиція по X (центр) */
    float y;          /* Позиція по Y (центр) */
    float width;      /* Ширина */
    float height;     /* Висота */
    uint32_t color;   /* Колір */
    bool isDragging;  /* Чи перетягується */
} DragRectGfx;

/**
 * @brief Ініціалізує курсор з параметрами.
 * @param screenW Ширина екрана (для розрахунку value та обмежень)
 */
CursorGfx InitCursor_GFX(float startX, float topY, float width, float height,
                        uint32_t color, int minValue, int maxValue, int screenW);

/**
 * @brief Оновлює стан усіх курсорів та центрального прямокутника.
 * @param mx, my Поточна позиція миші
 */
void UpdateAndHandleCursors_GFX(CursorGfx *cursors, int count, DragRectGfx *centerRect,
                               int mx, int my, int screenH,
                               bool mousePressed, bool mouseDown, bool mouseReleased);

/**
 * @brief Малює курсори, лінії, стрілки, ручку та відстань.
 * @param font Шрифт для тексту
 * @param spacing Відступ між символами
 */
void DrawCursorsAndDistance_GFX(CursorGfx *cursors, int count, DragRectGfx *centerRect,
                               RasterFont font, int spacing);

#endif /* CURSOR_GFX_H */
