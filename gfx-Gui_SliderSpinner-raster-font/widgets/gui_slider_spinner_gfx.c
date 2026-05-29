/* gui_slider_spinner_gfx.c */
#include "gui_slider_spinner_gfx.h"
#include "color_utils.h"
#include "graphics.h"
#include "gfx.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* ================= ГЛОБАЛЬНІ ЗМІННІ СТАНУ ================= */
static HoldStateGfx holdLeftStates[MAX_SPINNERS_GFX] = {{0}};
static HoldStateGfx holdRightStates[MAX_SPINNERS_GFX] = {{0}};
static bool widgetActive[MAX_SPINNERS_GFX] = {false};

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

static double GetSystemTimeGfx(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ts.tv_nsec / 1e9;
}

static bool PointInRectGfx(int px, int py, int rx, int ry, int rw, int rh) {
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

/* ✅ ВИПРАВЛЕНО: Заповнений трикутник замість ліній */
static void DrawArrowGfx(int cx, int cy, int size, ArrowDirectionGfx dir, uint32_t color) {
    int x1, y1, x2, y2, x3, y3;
    
    switch(dir) {
        case ARROW_GFX_LEFT:
            x1 = cx + size; y1 = cy - size;
            x2 = cx - size; y2 = cy;
            x3 = cx + size; y3 = cy + size;
            break;
        case ARROW_GFX_RIGHT:
            x1 = cx - size; y1 = cy - size;
            x2 = cx + size; y2 = cy;
            x3 = cx - size; y3 = cy + size;
            break;
        case ARROW_GFX_UP:
            x1 = cx - size; y1 = cy + size;
            x2 = cx;       y2 = cy - size;
            x3 = cx + size; y3 = cy + size;
            break;
        case ARROW_GFX_DOWN:
            x1 = cx - size; y1 = cy - size;
            x2 = cx;       y2 = cy + size;
            x3 = cx + size; y3 = cy - size;
            break;
        default: return;
    }
    DrawTriangle(x1, y1, x2, y2, x3, y3, color);
}

static int MeasureTextGfx(RasterFont font, const char *text, int spacing) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + spacing) - spacing;
}

/* ... (Функції IncrementValueGfx, DecrementValueGfx, NormalizeValueGfx, UpdateValueFromMouseGfx без змін) ... */

static bool IncrementValueGfx(void *value, void *minVal, void *maxVal, float step, GuiSpinnerGfxValueType type) {
    bool changed = false;
    if(type == GUI_SPINNER_GFX_FLOAT) {
        float *v = (float*)value;
        float a = *(float*)minVal, b = *(float*)maxVal;
        bool rev = (a > b);
        float rMin = fminf(a,b), rMax = fmaxf(a,b);
        if(!rev) { if(*v + step <= rMax) { *v += step; changed = true; } else if(*v != rMax) { *v = rMax; changed = true; } }
        else    { if(*v - step >= rMin) { *v -= step; changed = true; } else if(*v != rMin) { *v = rMin; changed = true; } }
    } else {
        int *v = (int*)value;
        int a = *(int*)minVal, b = *(int*)maxVal;
        bool rev = (a > b);
        int rMin = (a<b)?a:b, rMax = (a>b)?a:b;
        int s = (int)(step+0.5f);
        if(!rev) { if(*v + s <= rMax) { *v += s; changed = true; } else if(*v != rMax) { *v = rMax; changed = true; } }
        else    { if(*v - s >= rMin) { *v -= s; changed = true; } else if(*v != rMin) { *v = rMin; changed = true; } }
    }
    return changed;
}

static bool DecrementValueGfx(void *value, void *minVal, void *maxVal, float step, GuiSpinnerGfxValueType type) {
    bool changed = false;
    if(type == GUI_SPINNER_GFX_FLOAT) {
        float *v = (float*)value;
        float a = *(float*)minVal, b = *(float*)maxVal;
        bool rev = (a > b);
        float rMin = fminf(a,b), rMax = fmaxf(a,b);
        if(!rev) { if(*v - step >= rMin) { *v -= step; changed = true; } else if(*v != rMin) { *v = rMin; changed = true; } }
        else    { if(*v + step <= rMax) { *v += step; changed = true; } else if(*v != rMax) { *v = rMax; changed = true; } }
    } else {
        int *v = (int*)value;
        int a = *(int*)minVal, b = *(int*)maxVal;
        bool rev = (a > b);
        int rMin = (a<b)?a:b, rMax = (a>b)?a:b;
        int s = (int)(step+0.5f);
        if(!rev) { if(*v - s >= rMin) { *v -= s; changed = true; } else if(*v != rMin) { *v = rMin; changed = true; } }
        else    { if(*v + s <= rMax) { *v += s; changed = true; } else if(*v != rMax) { *v = rMax; changed = true; } }
    }
    return changed;
}

static float NormalizeValueGfx(void *value, void *minVal, void *maxVal, GuiSpinnerGfxValueType type) {
    if(type == GUI_SPINNER_GFX_FLOAT) {
        float v = *(float*)value, a = *(float*)minVal, b = *(float*)maxVal;
        float rMin = fminf(a,b), rMax = fmaxf(a,b);
        float n = (rMax==rMin)?0.0f:(v-rMin)/(rMax-rMin);
        return (a>b)?(1.0f-n):n;
    } else {
        int v = *(int*)value, a = *(int*)minVal, b = *(int*)maxVal;
        int rMin = (a<b)?a:b, rMax = (a>b)?a:b;
        float n = (rMax==rMin)?0.0f:(float)(v-rMin)/(float)(rMax-rMin);
        return (a>b)?(1.0f-n):n;
    }
}

static void UpdateValueFromMouseGfx(int pos, int minPos, int maxPos, void *value,
                                     void *minVal, void *maxVal, float step,
                                     GuiSpinnerGfxValueType type, GuiSpinnerGfxOrientation orient) {
    float norm = (float)(pos - minPos) / (float)(maxPos - minPos);
    if(norm < 0) norm = 0; if(norm > 1) norm = 1;
    if(orient == GUI_SPINNER_GFX_VERTICAL) norm = 1.0f - norm;
    
    if(type == GUI_SPINNER_GFX_FLOAT) {
        float a = *(float*)minVal, b = *(float*)maxVal;
        float rMin = fminf(a,b), rMax = fmaxf(a,b);
        if(a > b) norm = 1.0f - norm;
        float raw = rMin + norm * (rMax - rMin);
        float stepped = rMin + step * roundf((raw - rMin) / step);
        if(stepped < rMin) stepped = rMin; if(stepped > rMax) stepped = rMax;
        *(float*)value = stepped;
    } else {
        int a = *(int*)minVal, b = *(int*)maxVal;
        int rMin = (a<b)?a:b, rMax = (a>b)?a:b;
        if(a > b) norm = 1.0f - norm;
        int raw = rMin + (int)(norm * (rMax - rMin));
        int s = (int)(step+0.5f); if(s==0) s=1;
        int stepped = rMin + ((raw - rMin + s/2) / s) * s;
        if(stepped < rMin) stepped = rMin; if(stepped > rMax) stepped = rMax;
        *(int*)value = stepped;
    }
}

static bool ArrowButtonGfx(int x, int y, int w, int h, ArrowDirectionGfx dir,
                           void *value, void *minVal, void *maxVal, float step,
                           GuiSpinnerGfxValueType type, HoldStateGfx *hold,
                           uint32_t baseColor, GuiSpinnerGfxOrientation orient) {
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    bool over = PointInRectGfx(mx, my, x, y, w, h);
    bool changed = false;
    
    uint32_t btnColor = baseColor;
    if(over) btnColor = 0xD0D0D0;
    if(over && gfx_is_mouse_down(GFX_MOUSE_LEFT)) btnColor = 0xA0A0A0;
    
    DrawRectangleFast(x, y, w, h, btnColor);
    DrawRectangleLines(x, y, w, h, GetContrastColor(btnColor));
    
    ArrowDirectionGfx drawDir = dir;
    if(orient == GUI_SPINNER_GFX_VERTICAL) {
        if(dir == ARROW_GFX_LEFT) drawDir = ARROW_GFX_UP;
        else if(dir == ARROW_GFX_RIGHT) drawDir = ARROW_GFX_DOWN;
    }
    DrawArrowGfx(x + w/2, y + h/2, w/3, drawDir, GetContrastInvertColor(btnColor));
    
    double now = GetSystemTimeGfx();
    if(!hold->isHeld) { hold->lastUpdateTime = now; hold->accumulatedTime = 0; }
    double dt = now - hold->lastUpdateTime;
    hold->lastUpdateTime = now;
    
    bool pressed = over && gfx_is_mouse_pressed(GFX_MOUSE_LEFT);
    bool down = over && gfx_is_mouse_down(GFX_MOUSE_LEFT);
    bool inc = (dir == ARROW_GFX_UP || dir == ARROW_GFX_RIGHT);
    
    const double delay = 0.175, baseInt = 0.25, minInt = 0.005, accel = 0.075;
    
    if(pressed) {
        hold->isHeld = true; hold->holdStartTime = now; hold->accumulatedTime = 0;
        changed = inc ? IncrementValueGfx(value, minVal, maxVal, step, type)
                      : DecrementValueGfx(value, minVal, maxVal, step, type);
    } else if(down && hold->isHeld) {
        double dur = now - hold->holdStartTime;
        double interval = (dur > delay) ? fmaxf(minInt, baseInt - (dur-delay)*accel) : baseInt;
        hold->accumulatedTime += dt;
        while(hold->accumulatedTime >= interval) {
            hold->accumulatedTime -= interval;
            changed = (inc ? IncrementValueGfx(value, minVal, maxVal, step, type)
                          : DecrementValueGfx(value, minVal, maxVal, step, type)) || changed;
        }
    } else { hold->isHeld = false; hold->accumulatedTime = 0; }
    
    return changed;
}

static void DrawSliderGfx(int x, int y, int w, int h, float norm, uint32_t color, GuiSpinnerGfxOrientation orient) {
    DrawRectangleFast(x, y, w, h, 0xE0E0E0);
    
    if(orient == GUI_SPINNER_GFX_HORIZONTAL) {
        int fillW = (int)(norm * w);
        DrawRectangleFast(x, y, fillW, h, 0xC0C0C0);
        int knobX = x + fillW - 5;
        DrawRectangleFast(knobX, y+2, 10, h-4, GetContrastInvertColor(color));
        DrawRectangleLines(knobX, y+2, 10, h-4, GetContrastColor(color));
    } else {
        int fillH = (int)(norm * h);
        DrawRectangleFast(x, y+h-fillH, w, fillH, 0xC0C0C0);
        int knobY = y + h - fillH - 5;
        DrawRectangleFast(x+2, knobY, w-4, 10, GetContrastInvertColor(color));
        DrawRectangleLines(x+2, knobY, w-4, 10, GetContrastColor(color));
    }
    DrawRectangleLines(x, y, w, h, GetContrastColor(color));
}

/* ================= ОСНОВНА ФУНКЦІЯ ВІДЖЕТА ================= */

int Gui_SliderSpinner_GFX(int id, int centerX, int centerY, int width, int height,
                          const char *textLeft, const char *textRight,
                          void *value, void *minValue, void *maxValue,
                          float step, GuiSpinnerGfxValueType valueType,
                          GuiSpinnerGfxOrientation orientation,
                          uint32_t baseColor, RasterFont font, int spacing,
                          bool showButtons) {
    if(id < 0 || id >= MAX_SPINNERS_GFX || !value) return 0;
    
    HoldStateGfx *holdL = &holdLeftStates[id], *holdR = &holdRightStates[id];
    bool changed = false;
    
    int btnSize = showButtons ? ((orientation == GUI_SPINNER_GFX_HORIZONTAL) ? height : width) : 0;
    int sliderW = (orientation == GUI_SPINNER_GFX_HORIZONTAL) ? (width - 2*btnSize) : width;
    int sliderH = (orientation == GUI_SPINNER_GFX_HORIZONTAL) ? height : (height - 2*btnSize);
    
    int posX = centerX - width/2, posY = centerY - height/2;
    
    int lx, ly, rx, ry, sx, sy;
    if(orientation == GUI_SPINNER_GFX_HORIZONTAL) {
        lx = posX; ly = posY;
        rx = posX + width - btnSize; ry = posY;
        sx = posX + btnSize; sy = posY;
    } else {
        rx = posX; ry = posY;
        lx = posX; ly = posY + height - btnSize;
        sx = posX; sy = posY + btnSize;
    }
    
    if(showButtons) {
        if(ArrowButtonGfx(lx, ly, btnSize, (orientation==GUI_SPINNER_GFX_HORIZONTAL)?height:btnSize,
                         (orientation==GUI_SPINNER_GFX_HORIZONTAL)?ARROW_GFX_LEFT:ARROW_GFX_DOWN,
                         value, minValue, maxValue, step, valueType, holdL, baseColor, orientation))
            changed = true;
        if(ArrowButtonGfx(rx, ry, btnSize, (orientation==GUI_SPINNER_GFX_HORIZONTAL)?height:btnSize,
                         (orientation==GUI_SPINNER_GFX_HORIZONTAL)?ARROW_GFX_RIGHT:ARROW_GFX_UP,
                         value, minValue, maxValue, step, valueType, holdR, baseColor, orientation))
            changed = true;
    }
    
    float norm = NormalizeValueGfx(value, minValue, maxValue, valueType);
    DrawSliderGfx(sx, sy, sliderW, sliderH, norm, baseColor, orientation);
    
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    bool sliderOver = PointInRectGfx(mx, my, sx, sy, sliderW, sliderH);
    
    if(sliderOver) {
        int wheel = gfx_get_mouse_wheel();
        if(wheel != 0) {
            float delta = step * wheel;
            if(valueType == GUI_SPINNER_GFX_FLOAT) {
                float *v = (float*)value;
                float a = *(float*)minValue, b = *(float*)maxValue;
                bool rev = (a > b);
                float rMin = fminf(a,b), rMax = fmaxf(a,b);
                *v += rev ? -delta : delta;
                if(*v < rMin) *v = rMin; if(*v > rMax) *v = rMax;
            } else {
                int *v = (int*)value;
                int a = *(int*)minValue, b = *(int*)maxValue;
                bool rev = (a > b);
                int rMin = (a<b)?a:b, rMax = (a>b)?a:b;
                int s = (int)(step+0.5f);
                *v += rev ? -s*wheel : s*wheel;
                if(*v < rMin) *v = rMin; if(*v > rMax) *v = rMax;
            }
            changed = true;
        }
    }
    
    if(gfx_is_mouse_pressed(GFX_MOUSE_LEFT) && sliderOver) widgetActive[id] = true;
    if(gfx_is_mouse_released(GFX_MOUSE_LEFT)) widgetActive[id] = false;
    
    if(widgetActive[id]) {
        if(orientation == GUI_SPINNER_GFX_HORIZONTAL)
            UpdateValueFromMouseGfx(mx, sx, sx+sliderW, value, minValue, maxValue, step, valueType, orientation);
        else
            UpdateValueFromMouseGfx(my, sy, sy+sliderH, value, minValue, maxValue, step, valueType, orientation);
        changed = true;
    }
    
    if (showButtons) {
        if (orientation == GUI_SPINNER_GFX_HORIZONTAL) {
            if (textRight && textRight[0]) {
                int tw = MeasureTextGfx(font, textRight, spacing);
                int tx = rx + btnSize/2 - tw/2;
                int ty = ry - font.glyph_height - 8;
                DrawTextWithAutoInvertedBackground(font, tx, ty, textRight, spacing, 1, baseColor, 4, 1);
            }
            if (textLeft && textLeft[0]) {
                int tw = MeasureTextGfx(font, textLeft, spacing);
                int tx = lx + btnSize/2 - tw/2;
                int ty = ly - font.glyph_height - 8;
                DrawTextWithAutoInvertedBackground(font, tx, ty, textLeft, spacing, 1, baseColor, 4, 1);
            }
        } else {
            if (textLeft && textLeft[0]) {
                int tw = MeasureTextGfx(font, textLeft, spacing);
                int tx = posX + width/2 - tw/2;
                int ty = posY - font.glyph_height - 8;
                DrawTextWithAutoInvertedBackground(font, tx, ty, textLeft, spacing, 1, baseColor, 4, 1);
            }
            if (textRight && textRight[0]) {
                int tw = MeasureTextGfx(font, textRight, spacing);
                int tx = posX + width/2 - tw/2;
                int ty = ly + btnSize + 8;
                DrawTextWithAutoInvertedBackground(font, tx, ty, textRight, spacing, 1, baseColor, 4, 1);
            }
        }
    }
    
    char valStr[32];
    if(valueType == GUI_SPINNER_GFX_FLOAT) snprintf(valStr, sizeof(valStr), "%.2f", *(float*)value);
    else snprintf(valStr, sizeof(valStr), "%d", *(int*)value);
    
    int tw = MeasureTextGfx(font, valStr, spacing);
    int tx = sx + sliderW/2 - tw/2;
    int ty = sy + sliderH/2 - font.glyph_height/2;
    DrawTextWithAutoInvertedBackground(font, tx, ty, valStr, spacing, 1, baseColor, 4, 1);
    
    return changed ? 1 : 0;
}
