// graphics.h
#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "color.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

extern uint16_t BACK_COLOR;
extern uint16_t POINT_COLOR;

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

// Базові піксельні алгоритми
void DrawLine(int x1, int y1, int x2, int y2, uint32_t color);
void DrawThinLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color);
void DrawThickLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color);
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thickness, uint32_t color);
void DrawRectangle(int16_t x, int16_t y, int16_t width, int16_t height, uint32_t color);
void DrawRectangleLines(int16_t x, int16_t y, int16_t width, int16_t height, uint32_t color);

// ⚡ Оптимізовані примітиви (подвійний буфер gfx)
void DrawRectangleFast(int16_t x, int16_t y, int16_t width, int16_t height, uint32_t color);
void DrawRectangleLinesFast(int16_t x, int16_t y, int16_t width, int16_t height, uint32_t color);

void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

#ifdef __cplusplus
}
#endif

#endif /* _GRAPHICS_H */
