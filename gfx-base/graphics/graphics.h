// graphics.h — тепер просто "агрегатор"
#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "gfx.h"  // А gfx.h вже включає lines.h + rects.h
#include <string.h>
#include <stdlib.h>

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

#endif
