/*
gui_slider_spinner_gfx.c
Адаптація Gui_SliderSpinner для X11/gfx
✅ Скалярний API: minVal/maxVal передаються як double, без union/void*
✅ Використовує DrawTriangle з graphics.h (без дублювання коду)
✅ Логіка інверсій збережена для сумісності з raylib-версією
*/

#include "gui_slider_spinner_gfx.h"
#include "gfx.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* ================= КОНСТАНТИ ================= */
#define AUTO_REPEAT_DELAY_SEC   0.175f
#define AUTO_REPEAT_BASE_SEC    0.250f
#define AUTO_REPEAT_MIN_SEC     0.005f
#define AUTO_REPEAT_ACCEL       0.075f
#define SLIDER_KNOB_SIZE        10
#define SLIDER_KNOB_OFFSET      5
#define TEXT_PADDING            4
#define TEXT_BORDER_THICKNESS   1

/* ================= ГЛОБАЛЬНІ ЗМІННІ СТАНУ ================= */
static HoldStateGfx holdLeftStates[MAX_SPINNERS_GFX]  = {{0}};
static HoldStateGfx holdRightStates[MAX_SPINNERS_GFX] = {{0}};
static bool widgetActive[MAX_SPINNERS_GFX] = {false};

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

static inline bool PointInRectGfx(int px, int py, int rx, int ry, int rw, int rh) {
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

static inline int MeasureTextGfx(RasterFont font, const char *text, int spacing) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + spacing) - spacing;
}

/* 🔹 Хелпер: безпечне отримання realMin/realMax та прапорця інверсії з double */
static inline void _GetRange(double minVal, double maxVal, double *rMin, double *rMax, bool *rev) {
    *rev = (minVal > maxVal);
    *rMin = fmin(minVal, maxVal);
    *rMax = fmax(minVal, maxVal);
}

/* ================= ЛОГІКА ЗМІНИ ЗНАЧЕНЬ (зі збереженою інверсією) ================= */

static inline bool IncrementValueGfx(void *value, double minVal, double maxVal,
                                     float step, GuiSpinnerGfxValueType type) {
    bool changed = false;
    double rMin, rMax; bool rev;
    _GetRange(minVal, maxVal, &rMin, &rMax, &rev);
    
    if (type == GUI_SPINNER_GFX_FLOAT) {
        float *v = (float*)value;
        if (!rev) {
            if (*v + step <= rMax) { *v += step; changed = true; }
            else if (*v != (float)rMax) { *v = (float)rMax; changed = true; }
        } else {
            if (*v - step >= rMin) { *v -= step; changed = true; }
            else if (*v != (float)rMin) { *v = (float)rMin; changed = true; }
        }
    } else {
        int *v = (int*)value;
        int s = (int)(step + 0.5f);
        if (!rev) {
            if (*v + s <= (int)rMax) { *v += s; changed = true; }
            else if (*v != (int)rMax) { *v = (int)rMax; changed = true; }
        } else {
            if (*v - s >= (int)rMin) { *v -= s; changed = true; }
            else if (*v != (int)rMin) { *v = (int)rMin; changed = true; }
        }
    }
    return changed;
}

static inline bool DecrementValueGfx(void *value, double minVal, double maxVal,
                                     float step, GuiSpinnerGfxValueType type) {
    bool changed = false;
    double rMin, rMax;
    bool rev;
    _GetRange(minVal, maxVal, &rMin, &rMax, &rev);
    
    if (type == GUI_SPINNER_GFX_FLOAT) {
        float *v = (float*)value;
        if (!rev) {
            if (*v - step >= rMin) { *v -= step; changed = true; }
            else if (*v != (float)rMin) { *v = (float)rMin; changed = true; }
        } else {
            if (*v + step <= rMax) { *v += step; changed = true; }
            else if (*v != (float)rMax) { *v = (float)rMax; changed = true; }
        }
    } else {
        int *v = (int*)value;
        int s = (int)(step + 0.5f);
        if (!rev) {
            if (*v - s >= (int)rMin) { *v -= s; changed = true; }
            else if (*v != (int)rMin) { *v = (int)rMin; changed = true; }
        } else {
            if (*v + s <= (int)rMax) { *v += s; changed = true; }
            else if (*v != (int)rMax) { *v = (int)rMax; changed = true; }
        }
    }
    return changed;
}

/**
 * 🔹 Нормалізація значення [0..1] для візуалізації
 * ✅ ЗБЕРЕЖЕНО: інверсія значення (-*value) для сумісності gfx ↔ raylib
 */
static float NormalizeValueGfx(void *value, double minVal, double maxVal,
                               GuiSpinnerGfxValueType type) {
    double rMin, rMax; bool rev;
    _GetRange(minVal, maxVal, &rMin, &rMax, &rev);
    
    if (type == GUI_SPINNER_GFX_FLOAT) {
        /* 🔹 ІНВЕРСІЯ ЗНАЧЕННЯ: ключова для сумісності з raylib */
        float v = - *(float*)value;
        float norm = (rMax == rMin) ? 0.0f : (float)((v - rMin) / (rMax - rMin));
        return rev ? (1.0f - norm) : norm;
    } else {
        /* 🔹 ІНВЕРСІЯ ЗНАЧЕННЯ: ключова для сумісності з raylib */
        int v = - *(int*)value;
        float norm = (rMax == rMin) ? 0.0f : (float)(v - rMin) / (float)(rMax - rMin);
        return rev ? (1.0f - norm) : norm;
    }
}

/**
 * 🔹 Оновлення значення з позиції миші
 * ✅ ЗБЕРЕЖЕНО: інверсія при записі (-stepped) для сумісності gfx ↔ raylib
 */
static void UpdateValueFromMouseGfx(int pos, int minPos, int maxPos, void *value,
                                    double minVal, double maxVal, float step,
                                    GuiSpinnerGfxValueType type, GuiSpinnerGfxOrientation orient) {
    if (maxPos == minPos) return;
    float norm = (float)(pos - minPos) / (float)(maxPos - minPos);
    if (norm < 0) norm = 0; if (norm > 1) norm = 1;
    
    if (orient == GUI_SPINNER_GFX_VERTICAL) norm = 1.0f - norm;
    
    double rMin, rMax; bool rev;
    _GetRange(minVal, maxVal, &rMin, &rMax, &rev);
    if (rev) norm = 1.0f - norm;
    
    if (type == GUI_SPINNER_GFX_FLOAT) {
        float raw = (float)rMin + norm * (float)(rMax - rMin);
        float stepped = (float)rMin + step * roundf((raw - (float)rMin) / step);
        if (stepped < rMin) stepped = (float)rMin;
        if (stepped > rMax) stepped = (float)rMax;
        
        /* 🔹 ІНВЕРСІЯ ПРИ ЗАПИСУ: ключова для сумісності з raylib */
        *(float*)value = -stepped;
    } else {
        int raw = (int)rMin + (int)(norm * (rMax - rMin));
        int s = (int)(step + 0.5f); if (s == 0) s = 1;
        int stepped = (int)rMin + ((raw - (int)rMin + s/2) / s) * s;
        if (stepped < (int)rMin) stepped = (int)rMin;
        if (stepped > (int)rMax) stepped = (int)rMax;
        
        /* 🔹 ІНВЕРСІЯ ПРИ ЗАПИСУ: ключова для сумісності з raylib */
        *(int*)value = -stepped;
    }
}

/* ================= КНОПКА-СТРІЛКА З АВТОПОВТОРОМ ================= */

static bool ArrowButtonGfx(int x, int y, int w, int h, ArrowDirectionGfx dir,
                           void *value, double minVal, double maxVal, float step,
                           GuiSpinnerGfxValueType type, HoldStateGfx *hold,
                           uint32_t baseColor, GuiSpinnerGfxOrientation orient) {
    
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    bool over = PointInRectGfx(mx, my, x, y, w, h);
    bool changed = false;
    
    uint32_t btnColor = baseColor;
    if (over) btnColor = ColorBrighten(baseColor, 1.15f);
    if (over && gfx_is_mouse_down(GFX_MOUSE_LEFT)) btnColor = ColorBrighten(baseColor, 0.85f);
    
    DrawRectangleFast(x, y, w, h, btnColor);
    DrawRectangleLinesFast(x, y, w, h, GetContrastColor(btnColor));
    
    /* 🔹 Вершини стрілки для DrawTriangle з graphics.h */
    int cx = x + w/2, cy = y + h/2, sz = w/3;
    int x1, y1, x2, y2, x3, y3;
    switch(dir) {
        case ARROW_GFX_LEFT:  x1=cx+sz; y1=cy-sz; x2=cx-sz; y2=cy; x3=cx+sz; y3=cy+sz; break;
        case ARROW_GFX_RIGHT: x1=cx-sz; y1=cy-sz; x2=cx+sz; y2=cy; x3=cx-sz; y3=cy+sz; break;
        case ARROW_GFX_UP:    x1=cx-sz; y1=cy+sz; x2=cx; y2=cy-sz; x3=cx+sz; y3=cy+sz; break;
        case ARROW_GFX_DOWN:  x1=cx-sz; y1=cy-sz; x2=cx; y2=cy+sz; x3=cx+sz; y3=cy-sz; break;
        default: return changed;
    }
    DrawTriangle(x1, y1, x2, y2, x3, y3, GetContrastInvertColor(btnColor));
    
    double now = GetTimeGfx();  // ✅ Використовуємо вбудовану функцію з gfx.h
    if (!hold->isHeld) { hold->lastUpdateTime = now; hold->accumulatedTime = 0; }
    double dt = now - hold->lastUpdateTime;
    hold->lastUpdateTime = now;
    
    bool pressed = over && gfx_is_mouse_pressed(GFX_MOUSE_LEFT);
    bool down = over && gfx_is_mouse_down(GFX_MOUSE_LEFT);
    bool incAction = (dir == ARROW_GFX_UP || dir == ARROW_GFX_RIGHT);
    
    if (pressed) {
        hold->isHeld = true; hold->holdStartTime = now; hold->accumulatedTime = 0;
        /* 🔹 ЗБЕРЕЖЕНО: swapped calls для сумісності gfx ↔ raylib */
        changed = incAction ? DecrementValueGfx(value, minVal, maxVal, step, type)
                           : IncrementValueGfx(value, minVal, maxVal, step, type);
    }
    else if (down && hold->isHeld) {
        double dur = now - hold->holdStartTime;
        double interval = (dur > AUTO_REPEAT_DELAY_SEC) 
                        ? fmaxf(AUTO_REPEAT_MIN_SEC, AUTO_REPEAT_BASE_SEC - (dur-AUTO_REPEAT_DELAY_SEC)*AUTO_REPEAT_ACCEL) 
                        : AUTO_REPEAT_BASE_SEC;
        hold->accumulatedTime += dt;
        while (hold->accumulatedTime >= interval) {
            hold->accumulatedTime -= interval;
            changed = (incAction ? DecrementValueGfx(value, minVal, maxVal, step, type)
                                : IncrementValueGfx(value, minVal, maxVal, step, type)) || changed;
        }
    }
    else { hold->isHeld = false; hold->accumulatedTime = 0; }
    
    return changed;
}

/* ================= МАЛЮВАННЯ СЛАЙДЕРА ================= */

static void DrawSliderGfx(int x, int y, int w, int h, float norm,
                          uint32_t color, GuiSpinnerGfxOrientation orient) {
    const uint32_t TRACK_BG   = ColorBrighten(color, 0.50f);
    const uint32_t TRACK_FILL = ColorBrighten(color, 0.90f);
    const uint32_t KNOB_COL   = GetContrastInvertColor(color);
    const uint32_t BORDER_COL = GetContrastColor(color);
    
    DrawRectangleFast(x, y, w, h, TRACK_BG);
    
    if (orient == GUI_SPINNER_GFX_HORIZONTAL) {
        int fillW = (int)(norm * w);
        DrawRectangleFast(x, y, fillW, h, TRACK_FILL);
        int knobX = x + fillW - SLIDER_KNOB_OFFSET;
        DrawRectangleFast(knobX, y+2, SLIDER_KNOB_SIZE, h-4, KNOB_COL);
        DrawRectangleLinesFast(knobX, y+2, SLIDER_KNOB_SIZE, h-4, BORDER_COL);
    } else {
        int fillH = (int)(norm * h);
        DrawRectangleFast(x, y + h - fillH, w, fillH, TRACK_FILL);
        int knobY = y + h - fillH - SLIDER_KNOB_OFFSET;
        DrawRectangleFast(x+2, knobY, w-4, SLIDER_KNOB_SIZE, KNOB_COL);
        DrawRectangleLinesFast(x+2, knobY, w-4, SLIDER_KNOB_SIZE, BORDER_COL);
    }
    DrawRectangleLinesFast(x, y, w, h, BORDER_COL);
}

/* ================= ОСНОВНА ФУНКЦІЯ ВІДЖЕТА ================= */

int Gui_SliderSpinner_GFX(int id, int centerX, int centerY, int width, int height,
                          const char *textLeft, const char *textRight,
                          void *value, double minVal, double maxVal,  // ✅ Скалярні min/max!
                          float step, GuiSpinnerGfxValueType valueType,
                          GuiSpinnerGfxOrientation orientation,
                          uint32_t baseColor, RasterFont font, int spacing,
                          bool showButtons) {
    
    if (id < 0 || id >= MAX_SPINNERS_GFX || !value) return 0;
    
    HoldStateGfx *holdL = &holdLeftStates[id], *holdR = &holdRightStates[id];
    bool changed = false;
    
    int btnSize = showButtons ? ((orientation == GUI_SPINNER_GFX_HORIZONTAL) ? height : width) : 0;
    int sliderW = (orientation == GUI_SPINNER_GFX_HORIZONTAL) ? (width - 2*btnSize) : width;
    int sliderH = (orientation == GUI_SPINNER_GFX_HORIZONTAL) ? height : (height - 2*btnSize);
    
    int posX = centerX - width/2, posY = centerY - height/2;
    int lx, ly, rx, ry, sx, sy;
    if (orientation == GUI_SPINNER_GFX_HORIZONTAL) {
        lx = posX; ly = posY; rx = posX + width - btnSize; ry = posY; sx = posX + btnSize; sy = posY;
    } else {
        rx = posX; ry = posY; lx = posX; ly = posY + height - btnSize; sx = posX; sy = posY + btnSize;
    }
    
    if (showButtons) {
        if (ArrowButtonGfx(lx, ly, btnSize, (orientation==GUI_SPINNER_GFX_HORIZONTAL)?height:btnSize,
                          (orientation==GUI_SPINNER_GFX_HORIZONTAL)?ARROW_GFX_LEFT:ARROW_GFX_DOWN,
                          value, minVal, maxVal, step, valueType, holdL, baseColor, orientation)) changed = true;
        if (ArrowButtonGfx(rx, ry, btnSize, (orientation==GUI_SPINNER_GFX_HORIZONTAL)?height:btnSize,
                          (orientation==GUI_SPINNER_GFX_HORIZONTAL)?ARROW_GFX_RIGHT:ARROW_GFX_UP,
                          value, minVal, maxVal, step, valueType, holdR, baseColor, orientation)) changed = true;
    }
    
    float norm = NormalizeValueGfx(value, minVal, maxVal, valueType);
    DrawSliderGfx(sx, sy, sliderW, sliderH, norm, baseColor, orientation);
    
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    bool sliderOver = PointInRectGfx(mx, my, sx, sy, sliderW, sliderH);
    
    if (sliderOver) {
        int wheel = gfx_get_mouse_wheel();
        if (wheel != 0) {
            double delta = step * wheel;
            double rMin, rMax; bool rev;
            _GetRange(minVal, maxVal, &rMin, &rMax, &rev);
            
            if (valueType == GUI_SPINNER_GFX_FLOAT) {
                float *v = (float*)value;
                *v += rev ? (float)delta : -(float)delta;
                if (*v < rMin) *v = (float)rMin; if (*v > rMax) *v = (float)rMax;
            } else {
                int *v = (int*)value;
                int s = (int)(step+0.5f);
                *v += rev ? (int)(s*wheel) : -(int)(s*wheel);
                if (*v < (int)rMin) *v = (int)rMin; if (*v > (int)rMax) *v = (int)rMax;
            }
            changed = true;
        }
    }
    
    if (gfx_is_mouse_pressed(GFX_MOUSE_LEFT) && sliderOver) widgetActive[id] = true;
    if (gfx_is_mouse_released(GFX_MOUSE_LEFT)) widgetActive[id] = false;
    
    if (widgetActive[id]) {
        if (orientation == GUI_SPINNER_GFX_HORIZONTAL)
            UpdateValueFromMouseGfx(mx, sx, sx+sliderW, value, minVal, maxVal, step, valueType, orientation);
        else
            UpdateValueFromMouseGfx(my, sy, sy+sliderH, value, minVal, maxVal, step, valueType, orientation);
        changed = true;
    }
    
    if (showButtons) {
        if (textRight && textRight[0]) {
            int tw = MeasureTextGfx(font, textRight, spacing);
            DrawTextWithAutoInvertedBackground(font, rx + btnSize/2 - tw/2, ry - font.glyph_height - 8, 
                                              textRight, spacing, 1, baseColor, TEXT_PADDING, TEXT_BORDER_THICKNESS);
        }
        if (textLeft && textLeft[0]) {
            int tw = MeasureTextGfx(font, textLeft, spacing);
            DrawTextWithAutoInvertedBackground(font, lx + btnSize/2 - tw/2, ly - font.glyph_height - 8, 
                                              textLeft, spacing, 1, baseColor, TEXT_PADDING, TEXT_BORDER_THICKNESS);
        }
    }
    
    char valStr[32];
    if (valueType == GUI_SPINNER_GFX_FLOAT) snprintf(valStr, sizeof(valStr), "%.2f", *(float*)value);
    else snprintf(valStr, sizeof(valStr), "%d", *(int*)value);
    
    int tw = MeasureTextGfx(font, valStr, spacing);
    DrawTextWithAutoInvertedBackground(font, sx + sliderW/2 - tw/2, sy + sliderH/2 - font.glyph_height/2, 
                                      valStr, spacing, 1, baseColor, TEXT_PADDING, TEXT_BORDER_THICKNESS);
    
    return changed ? 1 : 0;
}
