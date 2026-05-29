// cursor.c - gfx-версія, повністю портабельна (без display.h)

#include "cursor.h"
#include "color_utils.h"
#include "graphics.h"
#include "gfx.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

extern int spacing;  // Відступ між символами (з main.c)

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

static bool PointInRect(int px, int py, float rx, float ry, float rw, float rh) {
    return (px >= rx - rw/2 && px <= rx + rw/2 &&
            py >= ry - rh/2 && py <= ry + rh/2);
}

static void DrawFilledTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    DrawTriangle(x1, y1, x2, y2, x3, y3, color);
}

static int MeasureTextGfx(RasterFont font, const char *text, int scale) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + spacing) - spacing;
}

/* ================= ІНІЦІАЛІЗАЦІЯ ================= */

Cursor InitCursor(float startX, float topY, float width, float height,
                  uint32_t color, int minValue, int maxValue) {
    Cursor cursor;
    cursor.x = startX;
    cursor.topY = topY;
    cursor.width = width;
    cursor.height = height;
    cursor.color = color;
    cursor.isDragging = false;
    cursor.minValue = minValue;
    cursor.maxValue = maxValue;
    // ⚠️ min_X / max_X встановлюються ззовні перед першим використанням!
    cursor.min_X = 0;
    cursor.max_X = 800;  // тимчасове значення
    cursor.value = minValue + (int)((maxValue - minValue) * (startX / 800.0f));
    cursor.y = topY + height + THIN_LINE;
    return cursor;
}

/* ================= ОБРОБКА КУРСОРА ================= */

static bool IsMouseOverCursor(Vector2 mousePos, Cursor c) {
    int sticking = 2;
    return (mousePos.x > c.x - c.width * sticking && 
            mousePos.x < c.x + c.width * sticking &&
            mousePos.y > c.topY && 
            mousePos.y < c.topY + c.height);
}

static void UpdateCursorDrag(Cursor *c, Vector2 mousePos, Cursor *other) {
    c->x = mousePos.x;

    float minX = c->min_X + c->width / 2;
    float maxX = c->max_X - c->width / 2;
    float collision = c->width * 1.5f;

    if (other && c->isDragging && c != other) {
        if (c->x < other->x && c->x + collision > other->x)
            c->x = other->x - collision;
        else if (c->x > other->x && c->x - collision < other->x)
            c->x = other->x + collision;
    }

    if (c->x < minX) c->x = minX;
    if (c->x > maxX) c->x = maxX;

    c->value = c->minValue + (int)((c->maxValue - c->minValue) * 
                ((c->x - minX) / (maxX - minX)));
    
    if (c->value < c->minValue) c->value = c->minValue;
    if (c->value > c->maxValue) c->value = c->maxValue;

    c->y = c->topY + c->height + THIN_LINE;
}

/* ================= ПУБЛІЧНІ ФУНКЦІЇ (з параметрами екрану!) ================= */

void UpdateAndHandleCursors(Cursor *cursors, int count, Vector2 mousePos,
                            bool mouseButtonPressed, bool mouseButtonDown,
                            bool mouseButtonReleased) {
    for (int i = 0; i < count; ++i) {
        Cursor *c = &cursors[i];
        Cursor *other = (count > 1) ? &cursors[(i + 1) % count] : NULL;

        if (mouseButtonPressed && IsMouseOverCursor(mousePos, *c))
            c->isDragging = true;
        if (c->isDragging && mouseButtonDown)
            UpdateCursorDrag(c, mousePos, other);
        if (mouseButtonReleased)
            c->isDragging = false;
    }
}

bool IsMouseOverRect(Vector2 mousePos, DragRect rect) {
    int sticking = 2;
    return (mousePos.x > rect.x - rect.width/2 && 
            mousePos.x < rect.x + rect.width/2 &&
            mousePos.y > rect.y - rect.height * sticking && 
            mousePos.y < rect.y + rect.height * sticking);
}

/* ✅ ВИПРАВЛЕНО: додаємо screenHeight як параметр */
void UpdateAndHandleCenterRect(DragRect *rect, Vector2 mousePos,
                               bool mouseButtonPressed, bool mouseButtonDown,
                               bool mouseButtonReleased,
                               int screenHeight) {  // ✅ Параметр замість Display_Get_HEIGHT()
    if (mouseButtonPressed && IsMouseOverRect(mousePos, *rect))
        rect->isDragging = true;
    
    if (rect->isDragging && mouseButtonDown) {
        rect->y = mousePos.y;
        
        // ✅ Обмеження через параметр, а не глобальну функцію
        int minY = 10;
        int maxY = screenHeight - 10;
        
        if (rect->y < minY) rect->y = minY;
        if (rect->y > maxY) rect->y = maxY;
    }
    
    if (mouseButtonReleased)
        rect->isDragging = false;
}

/* ================= МАЛЮВАННЯ ================= */

void DrawCursorsAndDistance(Cursor *cursors, int count, RasterFont font,
                            DragRect *centerRect,
                            int screenWidth, int screenHeight) {
    
    // Оновлюємо обмеження курсорів
    for (int i = 0; i < count; ++i) {
        cursors[i].min_X = 0;
        cursors[i].max_X = screenWidth;
    }

    // Отримуємо стан миші через gfx polling
    Vector2 mousePos = { (float)gfx_get_mouse_x(), (float)gfx_get_mouse_y() };
    bool mousePressed = gfx_is_mouse_pressed(GFX_MOUSE_LEFT);
    bool mouseDown = gfx_is_mouse_down(GFX_MOUSE_LEFT);
    bool mouseReleased = gfx_is_mouse_released(GFX_MOUSE_LEFT);

    // Оновлення логіки курсорів
    UpdateAndHandleCursors(cursors, count, mousePos, mousePressed, mouseDown, mouseReleased);
    
    // ✅ Передаємо screenHeight у UpdateAndHandleCenterRect
    UpdateAndHandleCenterRect(centerRect, mousePos, mousePressed, mouseDown, 
                              mouseReleased, screenHeight);

    // Додаткове обмеження centerRect->y
    if (centerRect->y < 10) centerRect->y = 10;
    if (centerRect->y > screenHeight - 10) centerRect->y = screenHeight - 10;

    // Центруємо ручку між курсорами
    if (count >= 2)
        centerRect->x = (cursors[0].x + cursors[1].x) / 2;

    // Малюємо курсори та вертикальні лінії
    for (int i = 0; i < count; ++i) {
        Cursor c = cursors[i];
        DrawRectangleFast((int)(c.x - c.width/2), (int)c.topY, 
                         (int)c.width, (int)c.height, c.color);
        
        float lineY = centerRect->y;
        float lineStart = c.topY + c.height;
        if (lineStart < lineY) {
            gfx_set_color_uint32(LIGHTGRAY);
            gfx_line((int)c.x, (int)lineStart, (int)c.x, (int)lineY);
        }
    }

    // Горизонтальна лінія зі стрілками
    if (count >= 2) {
        Cursor a = cursors[0], b = cursors[1];
        int distance = abs((int)a.x - (int)b.x);
        float lineY = centerRect->y;

        gfx_set_color_uint32(LIGHTGRAY);
        gfx_line((int)a.x, (int)lineY, (int)b.x, (int)lineY);

        /* Стрілка на A - ЗАПОВНЕНА */
        int arrowA = (b.x > a.x) ? ARROW_SIZE : -ARROW_SIZE;
        DrawFilledTriangle((int)a.x, (int)lineY,
                          (int)a.x + arrowA, (int)lineY + arrowA/2,
                          (int)a.x + arrowA, (int)lineY - arrowA/2,
                          LIGHTGRAY);

        /* Стрілка на B - ЗАПОВНЕНА */
        int arrowB = (a.x > b.x) ? ARROW_SIZE : -ARROW_SIZE;
        DrawFilledTriangle((int)b.x, (int)lineY,
                          (int)b.x + arrowB, (int)lineY + arrowB/2,
                          (int)b.x + arrowB, (int)lineY - arrowB/2,
                          LIGHTGRAY);

        /* Центральна ручка */
        DrawRectangleFast((int)(centerRect->x - centerRect->width/2),
                         (int)(centerRect->y - centerRect->height/2),
                         (int)centerRect->width, (int)centerRect->height,
                         centerRect->color);

        // Текст відстані
        char distText[32];
        snprintf(distText, sizeof(distText), "%d px", distance);
        int textW = MeasureTextGfx(font, distText, 1);
        int midX = (int)((a.x + b.x) / 2);
        int textY = (int)(lineY - font.glyph_height - 20);
        
        // Фон під текстом
        uint32_t bg = GetContrastInvertColor(LIGHTGRAY);
        int pad = 4;
        DrawRectangleFast(midX - textW/2 - pad, textY - pad,
                         textW + 2*pad, font.glyph_height + 2*pad, bg);
        DrawRectangleLinesFast(midX - textW/2 - pad, textY - pad,
                              textW + 2*pad, font.glyph_height + 2*pad, LIGHTGRAY);
        
        DrawTextScaled(font, midX - textW/2, textY, distText, spacing, 1, LIGHTGRAY);
    }

    // Значення курсорів
    if (count >= 1) {
        char valA[32], valB[32];
        snprintf(valA, sizeof(valA), "A:%d", cursors[0].value);
        DrawTextScaled(font, screenWidth - 120, 10, valA, spacing, 1, cursors[0].color);
        if (count >= 2) {
            snprintf(valB, sizeof(valB), "B:%d", cursors[1].value);
            DrawTextScaled(font, screenWidth - 60, 10, valB, spacing, 1, cursors[1].color);
        }
    }
}
