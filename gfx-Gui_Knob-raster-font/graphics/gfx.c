/*
A simple graphics library for CSE 20211 by Douglas Thain
This work is licensed under a Creative Commons Attribution 4.0 International License.
https://creativecommons.org/licenses/by/4.0/
*/

// CFLAGS += -D _POSIX_C_SOURCE=199309L # ця дефініція додана в makefile
#define _POSIX_C_SOURCE 199309L  // ⚠️ Обов'язково ПЕРЕД будь-якими #include!
#include "gfx.h"                 // ✅ Спочатку свій хедер (перевірка сигнатур)
#include "color_utils.h"         // ✅ Якщо потрібен лише для внутрішньої логіки
#include <time.h>                // ✅ Для clock_gettime
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

double GetTimeGfx(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ts.tv_nsec / 1e9;
}

/* ================= GLOBAL X11 STATE ================= */
static Display *gfx_display = 0;
static Window   gfx_window;
static GC       gfx_gc;
static Colormap gfx_colormap;
static int      gfx_fast_color_mode = 0;

/* Saved event coordinates */
static int saved_xpos = 0;
static int saved_ypos = 0;

/* ================= MOUSE STATE ================= */
static int m_x = 0, m_y = 0;
static int m_btn_curr = 0, m_btn_prev = 0;
static int m_wheel = 0;

/* ================= DOUBLE BUFFER STATE ================= */
static int double_buffer_enabled = 0;
static Pixmap gfx_backing_pixmap = None;
static GC     gfx_backing_gc = None;
static int    gfx_buf_width = 0, gfx_buf_height = 0;

/* ================= BACKGROUND COLOR STATE ================= */
static uint32_t gfx_bg_color = WHITE;  /* За замовчуванням білий фон */

/* Helper: safely set foreground color on a GC */
static void _set_gc_foreground(GC gc, uint32_t color) {
    XColor xcolor;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    if(gfx_fast_color_mode) {
        xcolor.pixel = ((b & 0xff) | ((g & 0xff) << 8) | ((r & 0xff) << 16));
    } else {
        xcolor.pixel = 0;
        xcolor.red = r << 8;
        xcolor.green = g << 8;
        xcolor.blue = b << 8;
        XAllocColor(gfx_display, gfx_colormap, &xcolor);
    }
    XSetForeground(gfx_display, gc, xcolor.pixel);
}

/* Helper: get current drawable/GC based on double buffer state */
static Drawable _get_target_drawable(void) {
    return (double_buffer_enabled && gfx_backing_pixmap != None) ? gfx_backing_pixmap : gfx_window;
}
static GC _get_target_gc(void) {
    return (double_buffer_enabled && gfx_backing_gc != None) ? gfx_backing_gc : gfx_gc;
}

/* ================= INIT ================= */
void gfx_open( int width, int height, const char *title )
{
    gfx_display = XOpenDisplay(0);
    if(!gfx_display) {
        fprintf(stderr,"gfx_open: unable to open the graphics window.\n");
        exit(1);
    }
    Visual *visual = DefaultVisual(gfx_display,0);
    if(visual && visual->class==TrueColor) {
        gfx_fast_color_mode = 1;
    } else {
        gfx_fast_color_mode = 0;
    }
    int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
    int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));
    
    gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display), 
                                     0, 0, width, height, 0, blackColor, blackColor);
    XSetWindowAttributes attr;
    attr.backing_store = Always;
    XChangeWindowAttributes(gfx_display, gfx_window, CWBackingStore, &attr);
    XStoreName(gfx_display, gfx_window, title);
    
    /* Full event mask for mouse + keyboard + window */
    XSelectInput(gfx_display, gfx_window, 
                 StructureNotifyMask | KeyPressMask | ButtonPressMask | 
                 ButtonReleaseMask | PointerMotionMask | ExposureMask);
    XMapWindow(gfx_display, gfx_window);
    
    gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);
    gfx_colormap = DefaultColormap(gfx_display, 0);
    XSetForeground(gfx_display, gfx_gc, whiteColor);
    
    /* Wait for MapNotify */
    for(;;) {
        XEvent e;
        XNextEvent(gfx_display, &e);
        if (e.type == MapNotify) break;
    }
    
    /* Init double buffer */
    gfx_buf_width = width;
    gfx_buf_height = height;
    gfx_backing_pixmap = XCreatePixmap(gfx_display, gfx_window, 
                                       width, height, DefaultDepth(gfx_display, 0));
    gfx_backing_gc = XCreateGC(gfx_display, gfx_backing_pixmap, 0, 0);
    XSetForeground(gfx_display, gfx_backing_gc, whiteColor);
}

/* ================= ORIGINAL DRAWING ================= */
void gfx_point( int x, int y ) {
    XDrawPoint(gfx_display, _get_target_drawable(), _get_target_gc(), x, y);
}

void DrawPixel(uint16_t x, uint16_t y, uint32_t color) {
    gfx_set_color_uint32(color);
    XDrawPoint(gfx_display, _get_target_drawable(), _get_target_gc(), x, y);
}

void gfx_line( int x1, int y1, int x2, int y2 ) {
    XDrawLine(gfx_display, _get_target_drawable(), _get_target_gc(), x1, y1, x2, y2);
}

/* Original RGB version */
void gfx_color_rgb( int r, int g, int b ) {
    uint32_t col = (r << 16) | (g << 8) | b;
    _set_gc_foreground(gfx_gc, col);
    if(double_buffer_enabled && gfx_backing_gc != None) {
        _set_gc_foreground(gfx_backing_gc, col);
    }
}

/* New uint32_t version */
void gfx_color_uint32(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    gfx_color_rgb(r, g, b);
}

void gfx_clear() {
    if (double_buffer_enabled && gfx_backing_pixmap != None) {
        /* Зберігаємо поточний колір */
        XGCValues vals;
        XGetGCValues(gfx_display, gfx_backing_gc, GCForeground, &vals);
        uint32_t saved_fg = vals.foreground;

        /* Очищуємо back-buffer фоновим кольором */
        gfx_set_color_uint32(gfx_bg_color);
        XFillRectangle(gfx_display, gfx_backing_pixmap, gfx_backing_gc, 
                       0, 0, gfx_buf_width, gfx_buf_height);

        /* Відновлюємо попередній колір */
        XSetForeground(gfx_display, gfx_backing_gc, saved_fg);
    } else {
        /* Без подвійного буферу: стандартне очищення вікна */
        XClearWindow(gfx_display, gfx_window);
    }
}

void gfx_clear_color( int r, int g, int b ) {
    gfx_bg_color = (r << 16) | (g << 8) | b;  /* Зберігаємо для подвійного буфера */
    XColor color;
    color.pixel = 0;
    color.red = r << 8;
    color.green = g << 8;
    color.blue = b << 8;
    XAllocColor(gfx_display, gfx_colormap, &color);
    XSetWindowAttributes attr;
    attr.background_pixel = color.pixel;
    XChangeWindowAttributes(gfx_display, gfx_window, CWBackPixel, &attr);
}

void gfx_clear_color_uint32(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    gfx_clear_color(r, g, b);
}

void gfx_set_background_color(uint32_t color) {
    gfx_clear_color_uint32(color);  // ✅ Одна реалізація для обох функцій
}

/* ================= EVENT HANDLING ================= */
int gfx_event_waiting() {
    XEvent event;
    gfx_flush();
    if(XCheckMaskEvent(gfx_display, KeyPressMask | ButtonPressMask, &event)) {
        XPutBackEvent(gfx_display, &event);
        return 1;
    }
    return 0;
}

char gfx_wait() {
    XEvent event;
    gfx_flush();
    while(1) {
        XNextEvent(gfx_display, &event);
        if(event.type == KeyPress) {
            saved_xpos = event.xkey.x;
            saved_ypos = event.xkey.y;
            return XLookupKeysym(&event.xkey, 0);
        } else if(event.type == ButtonPress) {
            saved_xpos = event.xbutton.x;
            saved_ypos = event.xbutton.y;
            return (char)event.xbutton.button;
        }
    }
}

int gfx_xpos() { return saved_xpos; }
int gfx_ypos() { return saved_ypos; }

int gfx_xsize() {
    XWindowAttributes wa;
    XGetWindowAttributes(gfx_display, gfx_window, &wa);
    return wa.width;
}
int gfx_ysize() {
    XWindowAttributes wa;
    XGetWindowAttributes(gfx_display, gfx_window, &wa);
    return wa.height;
}

void gfx_flush() { XFlush(gfx_display); }

/* ================= MOUSE POLLING ================= */
void gfx_process_events(void) {
    m_btn_prev = m_btn_curr;
    m_btn_curr = 0;
    XEvent ev;
    
    /* Drain queue without blocking */
    while (XCheckMaskEvent(gfx_display, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, &ev)) {
        if (ev.type == MotionNotify) {
            m_x = ev.xmotion.x;
            m_y = ev.xmotion.y;
        } else if (ev.type == ButtonPress) {
            if (ev.xbutton.button == 1) m_btn_curr |= 1;
            if (ev.xbutton.button == 3) m_btn_curr |= 2;
            if (ev.xbutton.button == 4) m_wheel = 1;
            if (ev.xbutton.button == 5) m_wheel = -1;
        } else if (ev.type == ButtonRelease) {
            if (ev.xbutton.button == 1) m_btn_curr &= ~1;
            if (ev.xbutton.button == 3) m_btn_curr &= ~2;
        }
    }
    
    /* Fallback: query exact pointer state if no motion arrived */
    Window r, c; int rx, ry, wx, wy; unsigned int mask;
    if (XQueryPointer(gfx_display, gfx_window, &r, &c, &rx, &ry, &wx, &wy, &mask)) {
        m_x = wx; m_y = wy;
        if (mask & Button1Mask) m_btn_curr |= 1; else m_btn_curr &= ~1;
        if (mask & Button3Mask) m_btn_curr |= 2; else m_btn_curr &= ~2;
    }
}

int  gfx_get_mouse_x(void)         { return m_x; }
int  gfx_get_mouse_y(void)         { return m_y; }
int  gfx_is_mouse_pressed(int btn) { return (m_btn_curr & btn) && !(m_btn_prev & btn); }
int  gfx_is_mouse_released(int btn){ return !(m_btn_curr & btn) && (m_btn_prev & btn); }
int  gfx_is_mouse_down(int btn)    { return (m_btn_curr & btn) != 0; }
int  gfx_get_mouse_wheel(void)     { int w = m_wheel; m_wheel = 0; return w; }

/* ================= DOUBLE BUFFER & FAST DRAWING ================= */
void gfx_enable_double_buffering(int enabled) {
    double_buffer_enabled = enabled;
}

void gfx_swap_buffers(void) {
    if(double_buffer_enabled && gfx_backing_pixmap != None) {
        GC temp = XCreateGC(gfx_display, gfx_window, 0, 0);
        XCopyArea(gfx_display, gfx_backing_pixmap, gfx_window, temp, 
                  0, 0, gfx_buf_width, gfx_buf_height, 0, 0);
        XFreeGC(gfx_display, temp);
        XFlush(gfx_display);
    }
}

void gfx_draw_fast_rect(int x, int y, int w, int h, uint32_t color) {
    Drawable d = _get_target_drawable();
    GC gc = _get_target_gc();
    gfx_set_color_uint32(color);
    XFillRectangle(gfx_display, d, gc, x, y, w, h);
}

void gfx_draw_fast_rect_outline(int x, int y, int w, int h, uint32_t color) {
    Drawable d = _get_target_drawable();
    GC gc = _get_target_gc();
    gfx_set_color_uint32(color);
    XDrawRectangle(gfx_display, d, gc, x, y, w, h);
}

/* ================= COLOR UTILS INLINE IMPLEMENTATION ================= */
static inline uint32_t gfx_get_contrast_text_color(uint32_t bg_color) {
    return GetContrastColor(bg_color);
}
