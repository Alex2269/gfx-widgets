/* cursor_vertical_gfx.h */
#ifndef CURSOR_VERTICAL_GFX_H
#define CURSOR_VERTICAL_GFX_H

#include "gfx.h"
#include "glyphs.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define STICKY_DISTANCE_GFX 10.0f
#define MAX_ACTIVE_CURSORS_GFX 16

/**
 * @brief Структура вертикального курсора (ручки).
 */
typedef struct {
    float x;          /* Фіксована X позиція */
    float y;          /* Позиція по Y (рухається) */
    float width;      /* Ширина ручки */
    float height;     /* Висота ручки */
    uint32_t color;   /* Колір ручки (0xRRGGBB) */
    bool isDragging;  /* Стан перетягування */
    int value;        /* Поточне значення */
    int minValue;     /* Мін. значення */
    int maxValue;     /* Макс. значення */
} CursorVerticalGfx;

/**
 * @brief Динамічний масив вертикальних курсорів.
 */
typedef struct {
    CursorVerticalGfx *items;
    int count;
    int capacity;
} CursorVerticalArrayGfx;

/**
 * @brief Стек індексів активних курсорів.
 */
typedef struct {
    int indices[MAX_ACTIVE_CURSORS_GFX];
    int top;  /* -1 якщо порожній */
} ActiveCursorStackGfx;

/* --- Управління масивом курсорів --- */
void InitCursorVerticalArrayGfx(CursorVerticalArrayGfx *arr, int initialCapacity);
void FreeCursorVerticalArrayGfx(CursorVerticalArrayGfx *arr);
void AddCursorVerticalGfx(CursorVerticalArrayGfx *arr, CursorVerticalGfx cursor);
void RemoveCursorVerticalGfx(CursorVerticalArrayGfx *arr, int index);

/* --- Ініціалізація --- */
CursorVerticalGfx InitCursorVerticalGfx(float fixedX, float startY, float width, float height,
                                        uint32_t color, int minValue, int maxValue);

/* ✅ ДОДАНО: Публічна ініціалізація стеку */
void InitActiveCursorStackGfx(ActiveCursorStackGfx *stack);

/* --- Логіка оновлення --- */
void UpdateAndHandleCursorVerticalsGfx(CursorVerticalArrayGfx *arr, 
                                       int screenHeight,
                                       bool mousePressed, bool mouseDown, bool mouseReleased,
                                       ActiveCursorStackGfx *activeStack);

/* --- Малювання --- */
void DrawCursorVerticalsGfx(CursorVerticalArrayGfx *arr, RasterFont font, int spacing);

#endif /* CURSOR_VERTICAL_GFX_H */
