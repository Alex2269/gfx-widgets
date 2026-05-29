/* cursor_vertical_gfx.c */
#include "cursor_vertical_gfx.h"
#include "graphics.h"
#include <stdlib.h>
#include <math.h>

/* ================= УПРАВЛІННЯ ПАМ'ЯТТЮ ================= */
void InitCursorVerticalArrayGfx(CursorVerticalArrayGfx *arr, int initialCapacity) {
    arr->items = (CursorVerticalGfx *)malloc(sizeof(CursorVerticalGfx) * initialCapacity);
    arr->count = 0;
    arr->capacity = initialCapacity;
}

void FreeCursorVerticalArrayGfx(CursorVerticalArrayGfx *arr) {
    if (arr->items) { free(arr->items); arr->items = NULL; }
    arr->count = 0;
    arr->capacity = 0;
}

void AddCursorVerticalGfx(CursorVerticalArrayGfx *arr, CursorVerticalGfx cursor) {
    if (arr->count >= arr->capacity) {
        arr->capacity *= 2;
        arr->items = (CursorVerticalGfx *)realloc(arr->items, sizeof(CursorVerticalGfx) * arr->capacity);
    }
    arr->items[arr->count++] = cursor;
}

void RemoveCursorVerticalGfx(CursorVerticalArrayGfx *arr, int index) {
    if (index < 0 || index >= arr->count) return;
    for (int i = index; i < arr->count - 1; i++) {
        arr->items[i] = arr->items[i + 1];
    }
    arr->count--;
}

/* ================= ІНІЦІАЛІЗАЦІЯ КУРСОРА ================= */
CursorVerticalGfx InitCursorVerticalGfx(float fixedX, float startY, float width, float height,
                                        uint32_t color, int minValue, int maxValue) {
    CursorVerticalGfx cursor;
    cursor.x = fixedX; cursor.y = startY;
    cursor.width = width; cursor.height = height;
    cursor.color = color; cursor.isDragging = false;
    cursor.minValue = minValue; cursor.maxValue = maxValue;
    return cursor;
}

/* ================= СТЕК АКТИВНИХ КУРСОРІВ ================= */

/* ✅ ВИПРАВЛЕНО: Прибрано 'static', тепер функція публічна */
void InitActiveCursorStackGfx(ActiveCursorStackGfx *stack) {
    stack->top = -1;
}

static void PushActiveCursorGfx(ActiveCursorStackGfx *stack, int idx) {
    if (stack->top < MAX_ACTIVE_CURSORS_GFX - 1) {
        stack->indices[++stack->top] = idx;
    }
}

static void PopActiveCursorGfx(ActiveCursorStackGfx *stack) {
    if (stack->top >= 0) stack->top--;
}

static bool IsCursorActiveGfx(ActiveCursorStackGfx *stack, int idx) {
    for (int i = 0; i <= stack->top; i++) {
        if (stack->indices[i] == idx) return true;
    }
    return false;
}

static int TopActiveCursorGfx(ActiveCursorStackGfx *stack) {
    if (stack->top >= 0) return stack->indices[stack->top];
    return -1;
}

/* ================= ДОПОМІЖНІ ФУНКЦІ ================= */

static bool IsMouseNearCursorVerticalGfx(int mx, int my, CursorVerticalGfx cursor) {
    return (mx > cursor.x - cursor.width - STICKY_DISTANCE_GFX &&
            mx < cursor.x + STICKY_DISTANCE_GFX &&
            my > cursor.y - cursor.height / 2 - STICKY_DISTANCE_GFX &&
            my < cursor.y + cursor.height / 2 + STICKY_DISTANCE_GFX);
}

static void UpdateCursorVerticalDragGfx(CursorVerticalGfx *cursor, int my, 
                                        CursorVerticalArrayGfx *arr, int currentIndex, int screenHeight) {
    float minY = cursor->height / 2;
    float maxY = screenHeight - cursor->height / 2;
    float desiredY = (float)my;

    if (desiredY < minY) desiredY = minY;
    if (desiredY > maxY) desiredY = maxY;

    for (int i = 0; i < arr->count; i++) {
        if (i == currentIndex) continue;
        CursorVerticalGfx *other = &arr->items[i];
        float dist = desiredY - other->y;
        float minDist = 1.0f;
        if (fabsf(dist) < minDist) {
            if (dist > 0) desiredY = other->y + minDist;
            else          desiredY = other->y - minDist;
        }
    }

    cursor->y = desiredY;
    float norm = (cursor->y - minY) / (maxY - minY);
    cursor->value = cursor->minValue + (int)((cursor->maxValue - cursor->minValue) * norm);
}

/* ================= ГОЛОВНІ ФУНКЦІЇ ================= */

void UpdateAndHandleCursorVerticalsGfx(CursorVerticalArrayGfx *arr, 
                                       int screenHeight,
                                       bool mousePressed, bool mouseDown, bool mouseReleased,
                                       ActiveCursorStackGfx *activeStack) {
    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();

    if (mousePressed) {
        for (int i = arr->count - 1; i >= 0; i--) {
            if (IsMouseNearCursorVerticalGfx(mx, my, arr->items[i])) {
                if (!IsCursorActiveGfx(activeStack, i)) {
                    CursorVerticalGfx temp = arr->items[i];
                    for (int j = i; j < arr->count - 1; j++) {
                        arr->items[j] = arr->items[j + 1];
                    }
                    arr->items[arr->count - 1] = temp;
                    
                    PushActiveCursorGfx(activeStack, arr->count - 1);
                    arr->items[arr->count - 1].isDragging = true;
                    break;
                }
            }
        }
    }

    if (mouseDown) {
        int activeIdx = TopActiveCursorGfx(activeStack);
        if (activeIdx >= 0 && activeIdx < arr->count) {
            UpdateCursorVerticalDragGfx(&arr->items[activeIdx], my, arr, activeIdx, screenHeight);
        }
    }

    if (mouseReleased) {
        int activeIdx = TopActiveCursorGfx(activeStack);
        if (activeIdx >= 0 && activeIdx < arr->count) {
            arr->items[activeIdx].isDragging = false;
            PopActiveCursorGfx(activeStack);
        }
    }
}

void DrawCursorVerticalsGfx(CursorVerticalArrayGfx *arr, RasterFont font, int spacing) {
    for (int i = 0; i < arr->count; i++) {
        CursorVerticalGfx cursor = arr->items[i];
        gfx_set_color_uint32(cursor.color);
        gfx_draw_fast_rect((int)(cursor.x - cursor.width), 
                           (int)(cursor.y - cursor.height / 2), 
                           (int)cursor.width, 
                           (int)cursor.height, 
                           cursor.color);

        gfx_set_color_uint32(0xC0C0C0);
        gfx_line((int)(cursor.x - cursor.width), (int)cursor.y, 
                 (int)(cursor.x - cursor.width - 20), (int)cursor.y);

        char valText[16];
        snprintf(valText, sizeof(valText), "%i", cursor.value);
        DrawTextScaled(font, (int)(cursor.x + 5), (int)(cursor.y - font.glyph_height/2), 
                       valText, spacing, 1, cursor.color);
    }
}
