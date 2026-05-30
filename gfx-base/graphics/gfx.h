/*
A simple graphics library for CSE 20211 by Douglas Thain
This work is licensed under a Creative Commons Attribution 4.0 International License.
https://creativecommons.org/licenses/by/4.0/
For complete documentation, see:
http://www.nd.edu/~dthain/courses/cse20211/fall2013/gfx
Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/
#ifndef GFX_H
#define GFX_H

/* ================= MODULES ================= */
#include "bezier.h"
#include "circles.h"
#include "ellipses.h"
#include "lines.h"
#include "polygons.h"
#include "rects.h"
#include "rounded_rect.h"
#include "triangles.h"
#include <stdint.h>
#include <stddef.h>

/* --- Mouse button constants (raylib-style) --- */
#define GFX_MOUSE_LEFT  1
#define GFX_MOUSE_RIGHT 2

/* Get system time in seconds (for timing operations) */
double GetTimeGfx(void);

/* ================= ORIGINAL GFX API ================= */
void gfx_open( int width, int height, const char *title );
void gfx_point( int x, int y );
void DrawPixel(uint16_t x, uint16_t y, uint32_t color);
void gfx_line( int x1, int y1, int x2, int y2 );

/* Change the current drawing color - supports both RGB and uint32_t */
void gfx_color_rgb( int red, int green, int blue );
void gfx_color_uint32( uint32_t color );

/* Macro to allow both: gfx_color(255,0,0) and gfx_color(0xFF0000) */
#define gfx_color(c, ...) _Generic((c), \
    uint32_t: gfx_color_uint32, \
    default: gfx_color_rgb \
)(c, ##__VA_ARGS__)

void gfx_clear();
void gfx_clear_color( int red, int green, int blue );
void gfx_clear_color_uint32( uint32_t color );
void gfx_set_background_color(uint32_t color);
char gfx_wait();
int gfx_xpos();
int gfx_ypos();
int gfx_xsize();
int gfx_ysize();
int gfx_event_waiting();
void gfx_flush();

/* ================= MOUSE POLLING API ================= */
/* Call once per frame BEFORE drawing. Non-blocking. */
void gfx_process_events(void);
int  gfx_get_mouse_x(void);
int  gfx_get_mouse_y(void);
int  gfx_is_mouse_pressed(int btn);
int  gfx_is_mouse_released(int btn);
int  gfx_is_mouse_down(int btn);
int  gfx_get_mouse_wheel(void);  /* Returns 1, -1 or 0. Consumed on call. */

/* ================= DOUBLE BUFFER & FAST DRAWING ================= */
void gfx_enable_double_buffering(int enabled);
void gfx_swap_buffers(void);

/* Internal-optimized primitives, exposed for graphics.c */
void gfx_draw_fast_rect(int x, int y, int w, int h, uint32_t color);
void gfx_draw_fast_rect_outline(int x, int y, int w, int h, uint32_t color);

/* ================= COLOR UTILS INTEGRATION (inline helpers) ================= */
/**
 * @brief Швидко встановлює колір для gfx з uint32_t (0xRRGGBB)
 */
static inline void gfx_set_color_uint32(uint32_t color) {
    gfx_color_uint32(color);
}

/**
 * @brief Отримує контрастний колір (чорний/білий) для тексту на заданому фоні
 */
static inline uint32_t gfx_get_contrast_text_color(uint32_t bg_color);

#endif /* GFX_H */
