// graphics.c
#include "graphics.h"
#include "gfx.h"

/* ==========================================
 *  SIMPLE LINE WITH COLOR
 *  ========================================== */
void DrawLine(int x1, int y1, int x2, int y2, uint32_t color) {
    gfx_set_color_uint32(color);  // Спочатку встановлюємо колір
    gfx_line(x1, y1, x2, y2);      // Потім малюємо лінію (4 аргументи)
}

// малювання лінії окремими пікселями з передачою кольору в функцію.
void DrawThinLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color)
{
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    float length = sqrt(delta_x * delta_x + delta_y * delta_y);
    float unit_x = delta_x / length;
    float unit_y = delta_y / length;
    float perp_x = -unit_y;
    float perp_y = unit_x;

    for (int i = -(thickness / 2); i <= thickness / 2; i++) {
        int offsetX = (int)(perp_x * i);
        int offsetY = (int)(perp_y * i);
        int x = x1;
        int y = y1;
        int xerr = 0, yerr = 0;
        int distance = (abs(delta_x) > abs(delta_y)) ? abs(delta_x) : abs(delta_y);

        for (int t = 0; t <= distance; t++) {
            DrawPixel(x + offsetX, y + offsetY, color);
            xerr += abs(delta_x);
            yerr += abs(delta_y);
            if (xerr > distance) { xerr -= distance; x += (delta_x > 0) ? 1 : -1; }
            if (yerr > distance) { yerr -= distance; y += (delta_y > 0) ? 1 : -1; }
        }
    }
}

// малювання лінії окремими квадратиками з передачою кольору в функцію.
void DrawThickLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color)
{
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    float length = sqrt(delta_x * delta_x + delta_y * delta_y);
    float unit_x = delta_x / length;
    float unit_y = delta_y / length;
    float perp_x = -unit_y;
    float perp_y = unit_x;
    float offset_perp_x = perp_x * thickness / 2.0f;
    float offset_perp_y = perp_y * thickness / 2.0f;

    for (int i = -(thickness / 2); i <= thickness / 2; i++) {
        int offsetX = (int)(offset_perp_x * i);
        int offsetY = (int)(offset_perp_y * i);
        int x = x1;
        int y = y1;
        int xerr = 0, yerr = 0;
        int distance = (abs(delta_x) > abs(delta_y)) ? abs(delta_x) : abs(delta_y);

        for (int t = 0; t <= distance; t++) {
            DrawRectangle(x + offsetX, y + offsetY, thickness, thickness, color);
            xerr += abs(delta_x);
            yerr += abs(delta_y);
            if (xerr > distance) { xerr -= distance; x += (delta_x > 0) ? 1 : -1; }
            if (yerr > distance) { yerr -= distance; y += (delta_y > 0) ? 1 : -1; }
        }
    }
}

// малювання лінії окремими квадратиками з передачою кольору в функцію.
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thickness, uint32_t color)
{
    int x1 = (int)startPos.x;
    int y1 = (int)startPos.y;
    int x2 = (int)endPos.x;
    int y2 = (int)endPos.y;
    
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    float length = sqrt(delta_x * delta_x + delta_y * delta_y);
    float unit_x = delta_x / length;
    float unit_y = delta_y / length;
    float perp_x = -unit_y;
    float perp_y = unit_x;
    float offset_perp_x = perp_x * thickness / 2.0f;
    float offset_perp_y = perp_y * thickness / 2.0f;

    for (int i = -(int)(thickness / 2); i <= (int)(thickness / 2); i++) {
        int offsetX = (int)(offset_perp_x * i);
        int offsetY = (int)(offset_perp_y * i);
        int x = x1;
        int y = y1;
        int xerr = 0, yerr = 0;
        int distance = (abs(delta_x) > abs(delta_y)) ? abs(delta_x) : abs(delta_y);

        for (int t = 0; t <= distance; t++) {
            DrawRectangle(x + offsetX, y + offsetY, (int)thickness, (int)thickness, color);
            xerr += abs(delta_x);
            yerr += abs(delta_y);
            if (xerr > distance) { xerr -= distance; x += (delta_x > 0) ? 1 : -1; }
            if (yerr > distance) { yerr -= distance; y += (delta_y > 0) ? 1 : -1; }
        }
    }
}

// Малювання заповненого прямокутника кольором color (у форматі 0xRRGGBB)
void DrawRectangle(int16_t x, int16_t y, int16_t width, int16_t height, uint32_t color)
{
    for (int16_t py = y; py < y + height; py++) {
        for (int16_t px = x; px < x + width; px++) {
            DrawPixel(px, py, color);
        }
    }
}

// Малювання не заповненого прямокутника кольором color (у форматі 0xRRGGBB)
void DrawRectangleLines(int16_t x, int16_t y, int16_t width, int16_t height, uint32_t color)
{
    for (int16_t px = x; px < x + width; px++) DrawPixel(px, y, color);
    for (int16_t px = x; px < x + width; px++) DrawPixel(px, y + height - 1, color);
    for (int16_t py = y; py < y + height; py++) DrawPixel(x, py, color);
    for (int16_t py = y; py < y + height; py++) DrawPixel(x + width - 1, py, color);
}

// ==========================================
// ⚡ OPTIMIZED PRIMITIVES (Double Buffer Aware)
// ==========================================

void DrawRectangleFast(int16_t x, int16_t y, int16_t width, int16_t height, uint32_t color) {
    if (width < 0) { x += width; width = -width; }
    if (height < 0) { y += height; height = -height; }
    gfx_draw_fast_rect(x, y, width, height, color);
}

void DrawRectangleLinesFast(int16_t x, int16_t y, int16_t width, int16_t height, uint32_t color) {
    if (width < 0) { x += width; width = -width; }
    if (height < 0) { y += height; height = -height; }
    gfx_draw_fast_rect_outline(x, y, width, height, color);
}

/* ==========================================
 *  ✅ ROBUST TRIANGLE FILL (Scanline Algorithm)
 *  Виправлено артефакти "Z-подібних" ліній
 *  ========================================== */
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    // 1. Сортуємо вершини за Y (p1 - найвища, p3 - найнижча)
    // p1
    int px1 = x1, py1 = y1;
    // p2
    int px2 = x2, py2 = y2;
    // p3
    int px3 = x3, py3 = y3;

    // Swap p1, p2
    if (py1 > py2) {
        int t = py1; py1 = py2; py2 = t;
        t = px1; px1 = px2; px2 = t;
    }
    // Swap p2, p3
    if (py2 > py3) {
        int t = py2; py2 = py3; py3 = t;
        t = px2; px2 = px3; px3 = t;
    }
    // Swap p1, p2 again to ensure total order
    if (py1 > py2) {
        int t = py1; py1 = py2; py2 = t;
        t = px1; px1 = px2; px2 = t;
    }

    // Встановлюємо колір
    gfx_set_color_uint32(color);

    // 2. Розраховуємо нахили (invslope = dx/dy)
    // Додаємо 1.0f, щоб уникнути ділення на 0 для горизонтальних ліній
    float invslope12 = (float)(px2 - px1) / (float)(py2 - py1 + 1);
    float invslope13 = (float)(px3 - px1) / (float)(py3 - py1 + 1);
    float invslope23 = (float)(px3 - px2) / (float)(py3 - py2 + 1);

    float curx1 = px1;
    float curx2 = px1;

    // 3. Малюємо верхню половину (від p1.y до p2.y)
    // Ліва сторона: p1->p2, Права сторона: p1->p3
    for (int scanline = py1; scanline <= py2; scanline++) {
        int start_x = (int)curx1;
        int end_x = (int)curx2;
        if (start_x > end_x) { int t = start_x; start_x = end_x; end_x = t; }
        gfx_line(start_x, scanline, end_x, scanline);
        curx1 += invslope12;
        curx2 += invslope13;
    }

    // 4. Малюємо нижню половину (від p2.y до p3.y)
    // Ліва сторона: p2->p3 (починаємо з p2.x), Права сторона: p1->p3 (продовжуємо)
    curx1 = px2;
    // curx2 вже вказує на поточну позицію на довгій стороні p1->p3

    // Коригуємо curx2, якщо p2 не лежить точно на лінії p1->p3 (через дискретність)
    // Але простіше просто продовжити з поточного curx2, який ми вже мали
    // Однак, щоб уникнути дірки, краще перерахувати curx2 відносно p1->p3 для y=py2
    curx2 = px1 + invslope13 * (py2 - py1);

    for (int scanline = py2; scanline <= py3; scanline++) {
        int start_x = (int)curx1;
        int end_x = (int)curx2;
        if (start_x > end_x) { int t = start_x; start_x = end_x; end_x = t; }
        gfx_line(start_x, scanline, end_x, scanline);
        curx1 += invslope23;
        curx2 += invslope13;
    }
}
