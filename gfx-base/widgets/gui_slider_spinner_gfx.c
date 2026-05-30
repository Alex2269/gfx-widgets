/*
gui_slider_spinner_gfx.c
Адаптація Gui_SliderSpinner для X11/gfx
✅ Логіка інверсій збережена для сумісності з raylib-версією
✅ Покращено стиль: константи, хелпери, коментарі
*/

#include "gui_slider_spinner_gfx.h"
#include "color_utils.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

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
static HoldStateGfx holdLeftStates[MAX_SPINNERS_GFX] = {{0}};
static HoldStateGfx holdRightStates[MAX_SPINNERS_GFX] = {{0}};
static bool widgetActive[MAX_SPINNERS_GFX] = {false};

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */

static inline double GetSystemTimeGfx(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ts.tv_nsec / 1e9;
}

static inline bool PointInRectGfx(int px, int py, int rx, int ry, int rw, int rh) {
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

/* Заповнений трикутник для стрілок */
static void DrawFilledTriangleGfx(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    if (y1 > y2) { int t=y1; y1=y2; y2=t; t=x1; x1=x2; x2=t; }
    if (y2 > y3) { int t=y2; y2=y3; y3=t; t=x2; x2=x3; x3=t; }
    if (y1 > y2) { int t=y1; y1=y2; y2=t; t=x1; x1=x2; x2=t; }
    
    if (y1 == y3) {
        int xa = (x1 < x3) ? x1 : x3, xb = (x1 > x3) ? x1 : x3;
        gfx_set_color_uint32(color);
        gfx_line(xa, y1, xb, y1);
        return;
    }
    
    float dx1 = (float)(x2 - x1) / (y2 - y1 + 1);
    float dx2 = (float)(x3 - x1) / (y3 - y1 + 1);
    float dx3 = (float)(x3 - x2) / (y3 - y2 + 1);
    
    float cx1 = x1, cx2 = x1;
    for (int y = y1; y <= y2; y++) {
        int xa = (int)cx1, xb = (int)cx2;
        if (xa > xb) { int t=xa; xa=xb; xb=t; }
        gfx_set_color_uint32(color);
        gfx_line(xa, y, xb, y);
        cx1 += dx1; cx2 += dx2;
    }
    cx1 = x2;
    for (int y = y2; y <= y3; y++) {
        int xa = (int)cx1, xb = (int)cx2;
        if (xa > xb) { int t=xa; xa=xb; xb=t; }
        gfx_set_color_uint32(color);
        gfx_line(xa, y, xb, y);
        cx1 += dx3; cx2 += dx2;
    }
}

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
    DrawFilledTriangleGfx(x1, y1, x2, y2, x3, y3, color);
}

static inline int MeasureTextGfx(RasterFont font, const char *text, int spacing) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + spacing) - spacing;
}

/* ================= ЛОГІКА ЗМІНИ ЗНАЧЕНЬ (зі збереженою інверсією) ================= */

/**
 * 🔹 Збільшує значення — зі збереженою логікою інверсії діапазону
 * ✅ КЛЮЧОВЕ: при min > max "інкремент" = зменшення значення (для сумісності з raylib)
 */
static inline bool IncrementValueGfx(void *value, void *minVal, void *maxVal, 
                                     float step, GuiSpinnerGfxValueType type) {
    bool changed = false;
    
    if (type == GUI_SPINNER_GFX_FLOAT) {
        float *v = (float*)value;
        float a = *(float*)minVal, b = *(float*)maxVal;
        bool reversed = (a > b);
        float realMin = fminf(a, b), realMax = fmaxf(a, b);
        
        if (!reversed) {
            if (*v + step <= realMax) { *v += step; changed = true; }
            else if (*v != realMax) { *v = realMax; changed = true; }
        } else {
            /* Інвертований діапазон: інкремент = віднімання */
            if (*v - step >= realMin) { *v -= step; changed = true; }
            else if (*v != realMin) { *v = realMin; changed = true; }
        }
    } else {
        int *v = (int*)value;
        int a = *(int*)minVal, b = *(int*)maxVal;
        bool reversed = (a > b);
        int realMin = (a < b) ? a : b, realMax = (a > b) ? a : b;
        int stepInt = (int)(step + 0.5f);
        
        if (!reversed) {
            if (*v + stepInt <= realMax) { *v += stepInt; changed = true; }
            else if (*v != realMax) { *v = realMax; changed = true; }
        } else {
            if (*v - stepInt >= realMin) { *v -= stepInt; changed = true; }
            else if (*v != realMin) { *v = realMin; changed = true; }
        }
    }
    return changed;
}

/**
 * 🔹 Зменшує значення — зі збереженою логікою інверсії діапазону
 */
static inline bool DecrementValueGfx(void *value, void *minVal, void *maxVal,
                                     float step, GuiSpinnerGfxValueType type) {
    bool changed = false;
    
    if (type == GUI_SPINNER_GFX_FLOAT) {
        float *v = (float*)value;
        float a = *(float*)minVal, b = *(float*)maxVal;
        bool reversed = (a > b);
        float realMin = fminf(a, b), realMax = fmaxf(a, b);
        
        if (!reversed) {
            if (*v - step >= realMin) { *v -= step; changed = true; }
            else if (*v != realMin) { *v = realMin; changed = true; }
        } else {
            /* Інвертований діапазон: декремент = додавання */
            if (*v + step <= realMax) { *v += step; changed = true; }
            else if (*v != realMax) { *v = realMax; changed = true; }
        }
    } else {
        int *v = (int*)value;
        int a = *(int*)minVal, b = *(int*)maxVal;
        bool reversed = (a > b);
        int realMin = (a < b) ? a : b, realMax = (a > b) ? a : b;
        int stepInt = (int)(step + 0.5f);
        
        if (!reversed) {
            if (*v - stepInt >= realMin) { *v -= stepInt; changed = true; }
            else if (*v != realMin) { *v = realMin; changed = true; }
        } else {
            if (*v + stepInt <= realMax) { *v += stepInt; changed = true; }
            else if (*v != realMax) { *v = realMax; changed = true; }
        }
    }
    return changed;
}

/**
 * 🔹 Нормалізація значення [0..1] для візуалізації
 * ✅ ЗБЕРЕЖЕНО: інверсія значення (-*value) для сумісності gfx ↔ raylib
 */
static float NormalizeValueGfx(void *value, void *minVal, void *maxVal, 
                               GuiSpinnerGfxValueType type) {
    if (type == GUI_SPINNER_GFX_FLOAT) {
        /* 🔹 ІНВЕРСІЯ ЗНАЧЕННЯ: ключова для сумісності з raylib */
        float v = - *(float*)value;
        float a = *(float*)minVal, b = *(float*)maxVal;
        float rMin = fminf(a, b), rMax = fmaxf(a, b);
        float norm = (rMax == rMin) ? 0.0f : (v - rMin) / (rMax - rMin);
        return (a > b) ? (1.0f - norm) : norm;
    } else {
        /* 🔹 ІНВЕРСІЯ ЗНАЧЕННЯ: ключова для сумісності з raylib */
        int v = - *(int*)value;
        int a = *(int*)minVal, b = *(int*)maxVal;
        int rMin = (a < b) ? a : b, rMax = (a > b) ? a : b;
        float norm = (rMax == rMin) ? 0.0f : (float)(v - rMin) / (float)(rMax - rMin);
        return (a > b) ? (1.0f - norm) : norm;
    }
}

/**
 * 🔹 Оновлення значення з позиції миші
 * ✅ ЗБЕРЕЖЕНО: інверсія при записі (-stepped) для сумісності gfx ↔ raylib
 */
static void UpdateValueFromMouseGfx(int pos, int minPos, int maxPos, void *value,
                                    void *minVal, void *maxVal, float step,
                                    GuiSpinnerGfxValueType type, 
                                    GuiSpinnerGfxOrientation orient) {
    float norm = (float)(pos - minPos) / (float)(maxPos - minPos);
    if (norm < 0) norm = 0; if (norm > 1) norm = 1;
    
    if (orient == GUI_SPINNER_GFX_VERTICAL) {
        norm = 1.0f - norm;
    }
    
    if (type == GUI_SPINNER_GFX_FLOAT) {
        float a = *(float*)minVal, b = *(float*)maxVal;
        float rMin = fminf(a, b), rMax = fmaxf(a, b);
        
        if (a > b) norm = 1.0f - norm;
        
        float raw = rMin + norm * (rMax - rMin);
        float stepped = rMin + step * roundf((raw - rMin) / step);
        if (stepped < rMin) stepped = rMin;
        if (stepped > rMax) stepped = rMax;
        
        /* 🔹 ІНВЕРСІЯ ПРИ ЗАПИСУ: ключова для сумісності з raylib */
        *(float*)value = -stepped;
        
    } else {
        int a = *(int*)minVal, b = *(int*)maxVal;
        int rMin = (a < b) ? a : b, rMax = (a > b) ? a : b;
        if (a > b) norm = 1.0f - norm;
        
        int raw = rMin + (int)(norm * (rMax - rMin));
        int s = (int)(step + 0.5f); if (s == 0) s = 1;
        int stepped = rMin + ((raw - rMin + s/2) / s) * s;
        if (stepped < rMin) stepped = rMin;
        if (stepped > rMax) stepped = rMax;
        
        /* 🔹 ІНВЕРСІЯ ПРИ ЗАПИСУ: ключова для сумісності з raylib */
        *(int*)value = -stepped;
    }
}

/* ================= КНОПКА-СТРІЛКА З АВТОПОВТОРОМ ================= */

static bool ArrowButtonGfx(int x, int y, int w, int h, ArrowDirectionGfx dir,
                           void *value, void *minVal, void *maxVal, float step,
                           GuiSpinnerGfxValueType type, HoldStateGfx *hold,
                           uint32_t baseColor, GuiSpinnerGfxOrientation orient) {
    
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    bool over = PointInRectGfx(mx, my, x, y, w, h);
    bool changed = false;
    
    uint32_t btnColor = baseColor;
    if (over) btnColor = ColorBrighten(baseColor, 1.15f);
    if (over && gfx_is_mouse_down(GFX_MOUSE_LEFT)) 
        btnColor = ColorBrighten(baseColor, 0.85f);
    
    DrawRectangleFast(x, y, w, h, btnColor);
    DrawRectangleLinesFast(x, y, w, h, GetContrastColor(btnColor));
    
    ArrowDirectionGfx drawDir = dir;
    if (orient == GUI_SPINNER_GFX_VERTICAL) {
        if (dir == ARROW_GFX_LEFT) drawDir = ARROW_GFX_UP;
        else if (dir == ARROW_GFX_RIGHT) drawDir = ARROW_GFX_DOWN;
    }
    DrawArrowGfx(x + w/2, y + h/2, w/3, drawDir, GetContrastInvertColor(btnColor));
    
    double now = GetSystemTimeGfx();
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
                        ? fmaxf(AUTO_REPEAT_MIN_SEC, 
                               AUTO_REPEAT_BASE_SEC - (dur-AUTO_REPEAT_DELAY_SEC)*AUTO_REPEAT_ACCEL) 
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

    /* 🔹 ІМІТАЦІЯ FADE: множник 0.4 для напівпрозорого фону */
    const uint32_t TRACK_BG = ColorBrighten(color, 0.70f);  /* Фон треку = baseColor × 0.7 */
    const uint32_t TRACK_FILL = ColorBrighten(color, 0.90f); /* Заповнення = яскравіше */

    const uint32_t KNOB_COLOR = GetContrastInvertColor(color);
    const uint32_t BORDER_COLOR = GetContrastColor(color);

    /* Малюємо фон треку */
    DrawRectangleFast(x, y, w, h, TRACK_BG);

    if (orient == GUI_SPINNER_GFX_HORIZONTAL) {
        /* Горизонтальний: заповнення зліва направо */
        int fillW = (int)(norm * w);
        DrawRectangleFast(x, y, fillW, h, TRACK_FILL);

        /* Ручка */
        int knobX = x + fillW - SLIDER_KNOB_OFFSET;
        DrawRectangleFast(knobX, y+2, SLIDER_KNOB_SIZE, h-4, KNOB_COLOR);
        DrawRectangleLinesFast(knobX, y+2, SLIDER_KNOB_SIZE, h-4, BORDER_COLOR);

    } else {
        /* Вертикальний: заповнення знизу вгору */
        int fillH = (int)(norm * h);
        DrawRectangleFast(x, y + h - fillH, w, fillH, TRACK_FILL);

        /* Ручка */
        int knobY = y + h - fillH - SLIDER_KNOB_OFFSET;
        DrawRectangleFast(x+2, knobY, w-4, SLIDER_KNOB_SIZE, GetContrastInvertColor(color));
        DrawRectangleLinesFast(x+2, knobY, w-4, SLIDER_KNOB_SIZE, BORDER_COLOR);
    }

    /* Рамка треку */
    DrawRectangleLinesFast(x, y, w, h, BORDER_COLOR);
}

/* ================= ОСНОВНА ФУНКЦІЯ ВІДЖЕТА ================= */

int Gui_SliderSpinner_GFX(int id, int centerX, int centerY, int width, int height,
                          const char *textLeft, const char *textRight,
                          void *value, void *minValue, void *maxValue,
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
        lx = posX; ly = posY;
        rx = posX + width - btnSize; ry = posY;
        sx = posX + btnSize; sy = posY;
    } else {
        rx = posX; ry = posY;
        lx = posX; ly = posY + height - btnSize;
        sx = posX; sy = posY + btnSize;
    }
    
    if (showButtons) {
        if (ArrowButtonGfx(lx, ly, btnSize, 
                          (orientation==GUI_SPINNER_GFX_HORIZONTAL)?height:btnSize,
                          (orientation==GUI_SPINNER_GFX_HORIZONTAL)?ARROW_GFX_LEFT:ARROW_GFX_DOWN,
                          value, minValue, maxValue, step, valueType, 
                          holdL, baseColor, orientation)) {
            changed = true;
        }
        if (ArrowButtonGfx(rx, ry, btnSize,
                          (orientation==GUI_SPINNER_GFX_HORIZONTAL)?height:btnSize,
                          (orientation==GUI_SPINNER_GFX_HORIZONTAL)?ARROW_GFX_RIGHT:ARROW_GFX_UP,
                          value, minValue, maxValue, step, valueType,
                          holdR, baseColor, orientation)) {
            changed = true;
        }
    }
    
    float norm = NormalizeValueGfx(value, minValue, maxValue, valueType);
    DrawSliderGfx(sx, sy, sliderW, sliderH, norm, baseColor, orientation);
    
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    bool sliderOver = PointInRectGfx(mx, my, sx, sy, sliderW, sliderH);
    
    if (sliderOver) {
        int wheel = gfx_get_mouse_wheel();
        if (wheel != 0) {
            float delta = step * wheel;
            
            if (valueType == GUI_SPINNER_GFX_FLOAT) {
                float *v = (float*)value;
                float a = *(float*)minValue, b = *(float*)maxValue;
                bool rev = (a > b);
                float rMin = fminf(a,b), rMax = fmaxf(a,b);
                
                /* 🔹 ЗБЕРЕЖЕНО: інвертована логіка колеса для сумісності з raylib */
                *v += rev ? delta : -delta;
                if (*v < rMin) *v = rMin; if (*v > rMax) *v = rMax;
                
            } else {
                int *v = (int*)value;
                int a = *(int*)minValue, b = *(int*)maxValue;
                bool rev = (a > b);
                int rMin = (a<b)?a:b, rMax = (a>b)?a:b;
                int s = (int)(step+0.5f);
                
                *v += rev ? s*wheel : -s*wheel;
                if (*v < rMin) *v = rMin; if (*v > rMax) *v = rMax;
            }
            changed = true;
        }
    }
    
    if (gfx_is_mouse_pressed(GFX_MOUSE_LEFT) && sliderOver) 
        widgetActive[id] = true;
    if (gfx_is_mouse_released(GFX_MOUSE_LEFT)) 
        widgetActive[id] = false;
    
    if (widgetActive[id]) {
        if (orientation == GUI_SPINNER_GFX_HORIZONTAL) {
            UpdateValueFromMouseGfx(mx, sx, sx+sliderW, value, 
                                   minValue, maxValue, step, valueType, orientation);
        } else {
            UpdateValueFromMouseGfx(my, sy, sy+sliderH, value, 
                                   minValue, maxValue, step, valueType, orientation);
        }
        changed = true;
    }
    
    if (showButtons) {
        if (textRight && textRight[0]) {
            int tw = MeasureTextGfx(font, textRight, spacing);
            int tx = rx + btnSize/2 - tw/2;
            int ty = ry - font.glyph_height - 8;
            DrawTextWithAutoInvertedBackground(font, tx, ty, textRight, 
                                              spacing, 1, baseColor, 
                                              TEXT_PADDING, TEXT_BORDER_THICKNESS);
        }
        if (textLeft && textLeft[0]) {
            int tw = MeasureTextGfx(font, textLeft, spacing);
            int tx = lx + btnSize/2 - tw/2;
            int ty = ly - font.glyph_height - 8;
            DrawTextWithAutoInvertedBackground(font, tx, ty, textLeft, 
                                              spacing, 1, baseColor,
                                              TEXT_PADDING, TEXT_BORDER_THICKNESS);
        }
    }
    
    char valStr[32];
    if (valueType == GUI_SPINNER_GFX_FLOAT) 
        snprintf(valStr, sizeof(valStr), "%.2f", *(float*)value);
    else 
        snprintf(valStr, sizeof(valStr), "%d", *(int*)value);
    
    int tw = MeasureTextGfx(font, valStr, spacing);
    int tx = sx + sliderW/2 - tw/2;
    int ty = sy + sliderH/2 - font.glyph_height/2;
    
    DrawTextWithAutoInvertedBackground(font, tx, ty, valStr, 
                                      spacing, 1, baseColor,
                                      TEXT_PADDING, TEXT_BORDER_THICKNESS);
    
    return changed ? 1 : 0;
}
