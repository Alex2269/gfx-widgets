/* cursor_gfx.c */
#include "cursor_gfx.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <math.h>

/* Перевірка: чи миша над курсором (розширена зона захоплення) */
static bool IsMouseOverCursorGfx(int mx, int my, CursorGfx cursor) {
    int sticking = 2; /* Розширення зони по X */
    return (mx > cursor.x - cursor.width * sticking &&
            mx < cursor.x + cursor.width * sticking &&
            my > cursor.topY &&
            my < cursor.topY + cursor.height);
}

/* Перевірка: чи миша над прямокутником (ручкою) */
static bool IsMouseOverRectGfx(int mx, int my, DragRectGfx rect) {
    int sticking = 2;
    return (mx > rect.x - rect.width / 2 &&
            mx < rect.x + rect.width / 2 &&
            my > rect.y - rect.height * sticking &&
            my < rect.y + rect.height * sticking);
}

/* Ініціалізація курсора */
CursorGfx InitCursor_GFX(float startX, float topY, float width, float height,
                        uint32_t color, int minValue, int maxValue, int screenW) {
    CursorGfx cursor;
    cursor.x = startX;
    cursor.topY = topY;
    cursor.width = width;
    cursor.height = height;
    cursor.color = color;
    cursor.isDragging = false;
    cursor.minValue = minValue;
    cursor.maxValue = maxValue;
    
    /* Обмеження виходу за межі екрана */
    cursor.min_X = 20;
    cursor.max_X = screenW - 20;
    
    /* Розрахунок початкового значення пропорційно позиції */
    cursor.value = minValue + (maxValue - minValue) * 
                   ((startX - cursor.min_X) / (cursor.max_X - cursor.min_X));
    
    cursor.y = topY + height + THIN_LINE_GFX;
    return cursor;
}

/* Оновлення позиції курсора під час перетягування */
static void UpdateCursorDragGfx(CursorGfx *cursor, int mx, CursorGfx *otherCursor) {
    cursor->x = (float)mx;
    float minX = cursor->min_X + cursor->width / 2;
    float maxX = cursor->max_X - cursor->width / 2;
    float collisionThreshold = cursor->width * 1.5f;

    /* Уникнення зіткнення з іншим курсором */
    if (otherCursor && cursor->isDragging && cursor != otherCursor) {
        if (cursor->x < otherCursor->x && cursor->x + collisionThreshold > otherCursor->x) {
            cursor->x = otherCursor->x - collisionThreshold;
        } else if (cursor->x > otherCursor->x && cursor->x - collisionThreshold < otherCursor->x) {
            cursor->x = otherCursor->x + collisionThreshold;
        }
    }

    /* Обмеження меж */
    if (cursor->x < minX) cursor->x = minX;
    if (cursor->x > maxX) cursor->x = maxX;

    /* Перерахунок значення */
    cursor->value = cursor->minValue + (cursor->maxValue - cursor->minValue) *
                    ((cursor->x - minX) / (maxX - minX));

    cursor->y = cursor->topY + cursor->height + THIN_LINE_GFX;
}

/* Головна функція оновлення стану */
void UpdateAndHandleCursors_GFX(CursorGfx *cursors, int count, DragRectGfx *centerRect,
                               int mx, int my, int screenH,
                               bool mousePressed, bool mouseDown, bool mouseReleased) {
    /* Оновлення курсорів */
    for (int i = 0; i < count; ++i) {
        CursorGfx *cursor = &cursors[i];
        CursorGfx *otherCursor = (count > 1) ? &cursors[(i + 1) % count] : NULL;

        if (mousePressed && IsMouseOverCursorGfx(mx, my, *cursor)) {
            cursor->isDragging = true;
        }
        if (cursor->isDragging && mouseDown) {
            UpdateCursorDragGfx(cursor, mx, otherCursor);
        }
        if (mouseReleased) {
            cursor->isDragging = false;
        }
    }

    /* Оновлення центрального прямокутника (ручки) */
    if (mousePressed && IsMouseOverRectGfx(mx, my, *centerRect)) {
        centerRect->isDragging = true;
    }
    if (centerRect->isDragging && mouseDown) {
        centerRect->y = (float)my;
        if (centerRect->y < 10) centerRect->y = 10;
        if (centerRect->y > screenH - 10) centerRect->y = screenH - 10;
    }
    if (mouseReleased) {
        centerRect->isDragging = false;
    }

    /* Центрування ручки між курсорами */
    if (count >= 2) {
        centerRect->x = (cursors[0].x + cursors[1].x) / 2.0f;
    }
}

/* Малювання заповненого прямокутника */
static void DrawRectGfx(int x, int y, int w, int h, uint32_t color) {
    DrawRectangleFast(x, y, w, h, color);
}

/* Малювання тексту */
static void DrawTextGfx(RasterFont font, int x, int y, const char *text, int spacing, uint32_t color) {
    DrawTextScaled(font, x, y, text, spacing, 1, color);
}

/* Головна функція малювання */
void DrawCursorsAndDistance_GFX(CursorGfx *cursors, int count, DragRectGfx *centerRect,
                               RasterFont font, int spacing) {
    int mx = gfx_get_mouse_x();
    int my = gfx_get_mouse_y();
    bool mousePressed = gfx_is_mouse_pressed(GFX_MOUSE_LEFT);
    bool mouseDown = gfx_is_mouse_down(GFX_MOUSE_LEFT);
    bool mouseReleased = gfx_is_mouse_released(GFX_MOUSE_LEFT);

    /* Оновлення стану (включає оновлення позицій) */
    UpdateAndHandleCursors_GFX(cursors, count, centerRect, mx, my, 600, 
                              mousePressed, mouseDown, mouseReleased);

    /* Малювання курсорів та вертикальних ліній */
    for (int i = 0; i < count; ++i) {
        CursorGfx c = cursors[i];
        
        /* Прямокутник курсора */
        DrawRectGfx((int)(c.x - c.width / 2), (int)c.topY, (int)c.width, (int)c.height, c.color);

        /* Вертикальна лінія до горизонтальної */
        float lineLength = centerRect->y - (c.topY + c.height);
        if (lineLength < 0) lineLength = 0;
        
        gfx_set_color_uint32(0xC0C0C0); /* LIGHTGRAY */
        gfx_line((int)c.x, (int)(c.topY + c.height), (int)c.x, (int)(c.topY + c.height + lineLength));
    }

    /* Малювання горизонтальної лінії, стрілок, ручки та відстані */
    if (count >= 2) {
        CursorGfx a = cursors[0];
        CursorGfx b = cursors[1];
        float distance = fabsf(a.x - b.x);
        float lineY = centerRect->y;
        int middleX = (int)((a.x + b.x) / 2.0f);
        int middleY = (int)(lineY - 15);

        /* Горизонтальна лінія */
        gfx_set_color_uint32(0xC0C0C0);
        gfx_line((int)a.x, (int)lineY, (int)b.x, (int)lineY);

        /* Стрілки (трикутники) */
        float arrowAOffset = (b.x > a.x) ? ARROW_SIZE_GFX : -ARROW_SIZE_GFX;
        /* Ліва стрілка */
        gfx_line((int)a.x, (int)lineY, (int)(a.x + arrowAOffset), (int)(lineY + arrowAOffset / 2));
        gfx_line((int)a.x, (int)lineY, (int)(a.x + arrowAOffset), (int)(lineY - arrowAOffset / 2));
        gfx_line((int)(a.x + arrowAOffset), (int)(lineY + arrowAOffset / 2), (int)(a.x + arrowAOffset), (int)(lineY - arrowAOffset / 2));

        float arrowBOffset = (a.x > b.x) ? ARROW_SIZE_GFX : -ARROW_SIZE_GFX;
        /* Права стрілка */
        gfx_line((int)b.x, (int)lineY, (int)(b.x + arrowBOffset), (int)(lineY + arrowBOffset / 2));
        gfx_line((int)b.x, (int)lineY, (int)(b.x + arrowBOffset), (int)(lineY - arrowBOffset / 2));
        gfx_line((int)(b.x + arrowBOffset), (int)(lineY + arrowBOffset / 2), (int)(b.x + arrowBOffset), (int)(lineY - arrowBOffset / 2));

        /* Центральний прямокутник (ручка) */
        DrawRectGfx((int)(centerRect->x - centerRect->width / 2), 
                    (int)(centerRect->y - centerRect->height / 2),
                    (int)centerRect->width, (int)centerRect->height, centerRect->color);

        /* Текст відстані */
        char distanceText[32];
        snprintf(distanceText, sizeof(distanceText), "%i px", (int)distance);
        int tw = utf8_strlen(distanceText) * (font.glyph_width + spacing) - spacing;
        DrawTextGfx(font, middleX - tw / 2, middleY - font.glyph_height - 5, distanceText, spacing, 0xC0C0C0);
    }

    /* Значення курсорів (праворуч зверху) */
    if (count >= 2) {
        char bufA[16], bufB[16];
        snprintf(bufA, sizeof(bufA), "A:%i", cursors[0].value);
        snprintf(bufB, sizeof(bufB), "B:%i", cursors[1].value);
        DrawTextGfx(font, 500, 10, bufA, spacing, cursors[0].color);
        DrawTextGfx(font, 550, 10, bufB, spacing, cursors[1].color);
    }
}
