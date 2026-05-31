/*
gui_slider_spinner_gfx.c
✅ Повністю розділені блоки: WHEEL | BUTTONS | KNOB
✅ Незалежні прапорці enable/invert для кожного типу вводу
✅ Виправлено: слайдери не блокують один одного, напрямок drag інтуїтивний
*/

#include "gui_slider_spinner_gfx.h"
#include "color_utils.h"
#include "graphics.h"
#include "gfx.h"
#include "glyphs.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* ================= КОНСТАНТИ ================= */
#define AUTO_REPEAT_DELAY   0.175f
#define AUTO_REPEAT_BASE    0.250f
#define AUTO_REPEAT_MIN     0.005f
#define AUTO_REPEAT_ACCEL   0.075f
#define SLIDER_KNOB_SIZE    10
#define SLIDER_KNOB_OFFSET  5
#define TEXT_PADDING        4
#define TEXT_BORDER         1

/* ================= ГЛОБАЛЬНІ ЗМІННІ ================= */
static HoldStateGfx holdLeftStates[MAX_SPINNERS_GFX]  = {{0}};
static HoldStateGfx holdRightStates[MAX_SPINNERS_GFX] = {{0}};
static bool widgetActive[MAX_SPINNERS_GFX] = {false};

/* 🔹 ОПЦІЇ ПЕРЕТИЯГУВАННЯ (не в аргументах функції) */
static bool DragEnable = true;   /* Увімкнути перетягування ручки */
static bool DragInvert = false;  /* true → рух вправо/вгору = зменшення */

/* ================= ДОПОМІЖНІ ФУНКЦІЇ ================= */
static inline bool PointInRectGfx(int px, int py, int rx, int ry, int rw, int rh) {
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

static inline int MeasureTextGfx(RasterFont font, const char *text, int spacing) {
    int len = utf8_strlen(text);
    return len * (font.glyph_width + spacing) - spacing;
}

/* 🔹 Безпечне оновлення значення з clamp до діапазону */
static bool _SafeUpdateValue(void *value, double delta, void *minVal, void *maxVal, GuiSpinnerGfxValueType type) {
    double rMin = (type == GUI_SPINNER_GFX_FLOAT) ? *(float*)minVal : *(int*)minVal;
    double rMax = (type == GUI_SPINNER_GFX_FLOAT) ? *(float*)maxVal : *(int*)maxVal;
    double realMin = (rMin < rMax) ? rMin : rMax;
    double realMax = (rMin > rMax) ? rMin : rMax;
    bool changed = false;

    if (type == GUI_SPINNER_GFX_FLOAT) {
        float *v = (float*)value;
        float newVal = *v + (float)delta;
        if (newVal < realMin) { *v = (float)realMin; changed = true; }
        else if (newVal > realMax) { *v = (float)realMax; changed = true; }
        else if (fabsf(newVal - *v) > 1e-6f) { *v = newVal; changed = true; }
    } else {
        int *v = (int*)value;
        int stepI = (int)(fabs(delta) + 0.5f);
        int deltaI = (delta > 0) ? stepI : -stepI;
        int newVal = *v + deltaI;
        if (newVal < (int)realMin) { *v = (int)realMin; changed = true; }
        else if (newVal > (int)realMax) { *v = (int)realMax; changed = true; }
        else if (newVal != *v) { *v = newVal; changed = true; }
    }
    return changed;
}

/* ================= ОБРОБКА КОЛЕСА ================= */
static bool _ProcessWheel(void *value, void *minVal, void *maxVal, float step,
                          GuiSpinnerGfxValueType type, bool invert, int mx, int my,
                          int sx, int sy, int sw, int sh) {
    if (!PointInRectGfx(mx, my, sx, sy, sw, sh)) return false;
    int wheel = gfx_get_mouse_wheel();
    if (wheel == 0) return false;

    double dir = invert ? -1.0 : 1.0;
    return _SafeUpdateValue(value, step * wheel * dir, minVal, maxVal, type);
}

/* ================= ОБРОБКА КНОПОК ================= */
static bool _ProcessButtons(int x, int y, int w, int h, ArrowDirectionGfx dir,
                            void *value, void *minVal, void *maxVal, float step,
                            GuiSpinnerGfxValueType type, bool invert,
                            HoldStateGfx *hold, uint32_t baseColor, uint32_t orient) {
    int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
    bool over = PointInRectGfx(mx, my, x, y, w, h);
    bool changed = false;

    uint32_t btnColor = baseColor;
    if (over) btnColor = ColorBrighten(baseColor, 0.95f);
    if (over && gfx_is_mouse_down(GFX_MOUSE_LEFT)) btnColor = ColorBrighten(baseColor, 0.75f);

    DrawRectangleFast(x, y, w, h, btnColor);
    DrawRectangleLinesFast(x, y, w, h, GetContrastColor(btnColor));

    /* Малюємо стрілку */
    int cx = x + w/2, cy = y + h/2, sz = w/3;
    int x1, y1, x2, y2, x3, y3;
    ArrowDirectionGfx drawDir = dir;
    if (orient == GUI_SPINNER_GFX_VERTICAL) {
        if (dir == ARROW_GFX_LEFT) drawDir = ARROW_GFX_UP;
        else if (dir == ARROW_GFX_RIGHT) drawDir = ARROW_GFX_DOWN;
    }
    switch(drawDir) {
        case ARROW_GFX_LEFT:  x1=cx+sz; y1=cy-sz; x2=cx-sz; y2=cy; x3=cx+sz; y3=cy+sz; break;
        case ARROW_GFX_RIGHT: x1=cx-sz; y1=cy-sz; x2=cx+sz; y2=cy; x3=cx-sz; y3=cy+sz; break;
        case ARROW_GFX_UP:    x1=cx-sz; y1=cy+sz; x2=cx; y2=cy-sz; x3=cx+sz; y3=cy+sz; break;
        case ARROW_GFX_DOWN:  x1=cx-sz; y1=cy-sz; x2=cx; y2=cy+sz; x3=cx+sz; y3=cy-sz; break;
    }
    DrawTriangle(x1, y1, x2, y2, x3, y3, GetContrastInvertColor(btnColor));

    /* Таймінг */
    double now = GetTimeGfx();
    if (!hold->isHeld) { hold->lastUpdateTime = now; hold->accumulatedTime = 0; }
    double dt = now - hold->lastUpdateTime;
    hold->lastUpdateTime = now;

    bool pressed = over && gfx_is_mouse_pressed(GFX_MOUSE_LEFT);
    bool down = over && gfx_is_mouse_down(GFX_MOUSE_LEFT);
    bool incBase = (dir == ARROW_GFX_UP || dir == ARROW_GFX_RIGHT);
    int direction = (incBase ? 1 : -1) * (invert ? -1 : 1);

    if (pressed) {
        hold->isHeld = true; hold->holdStartTime = now; hold->accumulatedTime = 0;
        changed = _SafeUpdateValue(value, step * direction, minVal, maxVal, type);
    }
    else if (down && hold->isHeld) {
        double dur = now - hold->holdStartTime;
        double interval = (dur > AUTO_REPEAT_DELAY) 
                        ? fmaxf(AUTO_REPEAT_MIN, AUTO_REPEAT_BASE - (dur-AUTO_REPEAT_DELAY)*AUTO_REPEAT_ACCEL) 
                        : AUTO_REPEAT_BASE;
        hold->accumulatedTime += dt;
        while (hold->accumulatedTime >= interval) {
            hold->accumulatedTime -= interval;
            changed = _SafeUpdateValue(value, step * direction, minVal, maxVal, type) || changed;
        }
    }
    else { hold->isHeld = false; hold->accumulatedTime = 0; }

    return changed;
}

/* ================= ОБРОБКА РУЧКИ (DRAG) ================= */
static bool _ProcessKnob(int id, void *value, void *minVal, void *maxVal, float step,
                         GuiSpinnerGfxValueType type, bool invert, GuiSpinnerGfxOrientation orient,
                         int mx, int my, int sx, int sy, int sw, int sh) {
    /* ✅ ВИПРАВЛЕНО: використовуємо widgetActive[id] замість widgetActive[0] */
    if (gfx_is_mouse_pressed(GFX_MOUSE_LEFT) && PointInRectGfx(mx, my, sx, sy, sw, sh))
        widgetActive[id] = true;
    if (gfx_is_mouse_released(GFX_MOUSE_LEFT)) { widgetActive[id] = false; return false; }
    if (!widgetActive[id]) return false;

    double pos = (orient == GUI_SPINNER_GFX_HORIZONTAL) ? mx : my;
    double minP = (orient == GUI_SPINNER_GFX_HORIZONTAL) ? sx : sy;
    double maxP = (orient == GUI_SPINNER_GFX_HORIZONTAL) ? sx + sw : sy + sh;

    double norm = (pos - minP) / (maxP - minP);
    if (norm < 0.0) norm = 0.0; if (norm > 1.0) norm = 1.0;

    /* Для вертикального: Y росте вниз, тому інвертуємо, щоб рух вгору = більше значення */
    if (orient == GUI_SPINNER_GFX_VERTICAL) norm = 1.0 - norm;
    if (invert) norm = 1.0 - norm; /* Додаткова інверсія якщо DragInvert=true */

    double rMin = (type == GUI_SPINNER_GFX_FLOAT) ? *(float*)minVal : *(int*)minVal;
    double rMax = (type == GUI_SPINNER_GFX_FLOAT) ? *(float*)maxVal : *(int*)maxVal;
    double realMin = (rMin < rMax) ? rMin : rMax;
    double realMax = (rMin > rMax) ? rMin : rMax;
    double range = realMax - realMin;

    if (type == GUI_SPINNER_GFX_FLOAT) {
        float raw = (float)realMin + (float)(norm * range);
        float stepped = (float)realMin + step * roundf((raw - (float)realMin) / step);
        if (stepped < realMin) stepped = (float)realMin;
        if (stepped > realMax) stepped = (float)realMax;
        *(float*)value = stepped;
    } else {
        int raw = (int)realMin + (int)(norm * range);
        int s = (int)(step + 0.5f); if (s == 0) s = 1;
        int stepped = (int)realMin + ((raw - (int)realMin + s/2) / s) * s;
        if (stepped < (int)realMin) stepped = (int)realMin;
        if (stepped > (int)realMax) stepped = (int)realMax;
        *(int*)value = stepped;
    }
    return true;
}

/* ================= МАЛЮВАННЯ СЛАЙДЕРА ================= */
static void _DrawSlider(int x, int y, int w, int h, float norm, uint32_t color, GuiSpinnerGfxOrientation orient) {
    const uint32_t BG   = ColorBrighten(color, 0.50f);
    const uint32_t FILL = ColorBrighten(color, 0.90f);
    const uint32_t KNOB = GetContrastInvertColor(color);
    const uint32_t BORDER = GetContrastColor(color);

    DrawRectangleFast(x, y, w, h, BG);

    if (orient == GUI_SPINNER_GFX_HORIZONTAL) {
        int fw = (int)(norm * w); fw = fw < 0 ? 0 : (fw > w ? w : fw);
        DrawRectangleFast(x, y, fw, h, FILL);
        int kx = x + fw - SLIDER_KNOB_OFFSET;
        if (kx < x) kx = x; if (kx > x + w - SLIDER_KNOB_SIZE) kx = x + w - SLIDER_KNOB_SIZE;
        DrawRectangleFast(kx, y+2, SLIDER_KNOB_SIZE, h-4, KNOB);
        DrawRectangleLinesFast(kx, y+2, SLIDER_KNOB_SIZE, h-4, BORDER);
    } else {
        int fh = (int)(norm * h); fh = fh < 0 ? 0 : (fh > h ? h : fh);
        DrawRectangleFast(x, y + h - fh, w, fh, FILL);
        int ky = y + h - fh - SLIDER_KNOB_OFFSET;
        if (ky < y) ky = y; if (ky > y + h - SLIDER_KNOB_SIZE) ky = y + h - SLIDER_KNOB_SIZE;
        DrawRectangleFast(x+2, ky, w-4, SLIDER_KNOB_SIZE, KNOB);
        DrawRectangleLinesFast(x+2, ky, w-4, SLIDER_KNOB_SIZE, BORDER);
    }
    DrawRectangleLinesFast(x, y, w, h, BORDER);
}

/* ================= ОСНОВНА ФУНКЦІЯ ================= */
int Gui_SliderSpinner_GFX(int id, int centerX, int centerY, int width, int height,
                          const char *textLeft, const char *textRight,
                          void *value, void *minValue, void *maxValue,
                          float step, GuiSpinnerGfxValueType valueType,
                          GuiSpinnerGfxOrientation orientation,
                          uint32_t baseColor, RasterFont font, int spacing,
                          bool showButtons) {
    /* Локальні опції (можна змінити на static, якщо потрібно керувати глобально) */
    bool wheelEnable = true; bool wheelInvert = false;
    bool btnEnable   = true; bool btnInvert   = false;
    bool knobEnable  = DragEnable; bool knobInvert = DragInvert; /* ✅ Використовуємо наші опції */

    if (id < 0 || id >= MAX_SPINNERS_GFX || !value) return 0;

    HoldStateGfx *holdL = &holdLeftStates[id], *holdR = &holdRightStates[id];
    bool changed = false;

    int btnSize = showButtons ? ((orientation == GUI_SPINNER_GFX_HORIZONTAL) ? height : width) : 0;
    int sw = (orientation == GUI_SPINNER_GFX_HORIZONTAL) ? (width - 2*btnSize) : width;
    int sh = (orientation == GUI_SPINNER_GFX_HORIZONTAL) ? height : (height - 2*btnSize);
    int px = centerX - width/2, py = centerY - height/2;
    int lx, ly, rx, ry, sx, sy;

    if (orientation == GUI_SPINNER_GFX_HORIZONTAL) {
        lx = px; ly = py; rx = px + width - btnSize; ry = py; sx = px + btnSize; sy = py;
    } else {
        rx = px; ry = py; lx = px; ly = py + height - btnSize; sx = px; sy = py + btnSize;
    }

    /* ================= WHEEL ================= */
    if (wheelEnable) {
        int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
        if (_ProcessWheel(value, minValue, maxValue, step, valueType, wheelInvert, mx, my, sx, sy, sw, sh))
            changed = true;
    }

    /* ================= BUTTONS ================= */
    if (btnEnable && showButtons) {
        ArrowDirectionGfx lDir = (orientation == GUI_SPINNER_GFX_HORIZONTAL) ? ARROW_GFX_LEFT : ARROW_GFX_DOWN;
        ArrowDirectionGfx rDir = (orientation == GUI_SPINNER_GFX_HORIZONTAL) ? ARROW_GFX_RIGHT : ARROW_GFX_UP;
        if (_ProcessButtons(lx, ly, btnSize, (orientation==GUI_SPINNER_GFX_HORIZONTAL)?height:btnSize, lDir,
                            value, minValue, maxValue, step, valueType, btnInvert, holdL, baseColor, orientation)) changed = true;
        if (_ProcessButtons(rx, ry, btnSize, (orientation==GUI_SPINNER_GFX_HORIZONTAL)?height:btnSize, rDir,
                            value, minValue, maxValue, step, valueType, btnInvert, holdR, baseColor, orientation)) changed = true;
    }

    /* ================= KNOB (DRAG) ================= */
    if (knobEnable) {
        int mx = gfx_get_mouse_x(), my = gfx_get_mouse_y();
        /* ✅ ВИПРАВЛЕНО: передаємо id для незалежного керування станом */
        if (_ProcessKnob(id, value, minValue, maxValue, step, valueType, knobInvert, orientation, mx, my, sx, sy, sw, sh))
            changed = true;
    }

    /* ================= DRAW & TEXT ================= */
    double rMin = (valueType == GUI_SPINNER_GFX_FLOAT) ? *(float*)minValue : *(int*)minValue;
    double rMax = (valueType == GUI_SPINNER_GFX_FLOAT) ? *(float*)maxValue : *(int*)maxValue;
    double val = (valueType == GUI_SPINNER_GFX_FLOAT) ? *(float*)value : *(int*)value;
    double realMin = (rMin < rMax) ? rMin : rMax;
    double realMax = (rMin > rMax) ? rMin : rMax;
    float norm = (realMax == realMin) ? 0.0f : (float)((val - realMin) / (realMax - realMin));
    if (norm < 0.0f) norm = 0.0f; if (norm > 1.0f) norm = 1.0f;

    _DrawSlider(sx, sy, sw, sh, norm, baseColor, orientation);

    /* Текст */
    if (showButtons) {
        if (textRight && textRight[0]) {
            int tw = MeasureTextGfx(font, textRight, spacing);
            DrawTextWithAutoInvertedBackground(font, rx + btnSize/2 - tw/2, ry - font.glyph_height - 8, textRight, spacing, 1, baseColor, TEXT_PADDING, TEXT_BORDER);
        }
        if (textLeft && textLeft[0]) {
            int tw = MeasureTextGfx(font, textLeft, spacing);
            DrawTextWithAutoInvertedBackground(font, lx + btnSize/2 - tw/2, ly - font.glyph_height - 8, textLeft, spacing, 1, baseColor, TEXT_PADDING, TEXT_BORDER);
        }
    }
    char valStr[32];
    if (valueType == GUI_SPINNER_GFX_FLOAT) snprintf(valStr, sizeof(valStr), "%.2f", *(float*)value);
    else snprintf(valStr, sizeof(valStr), "%d", *(int*)value);
    int tw = MeasureTextGfx(font, valStr, spacing);
    DrawTextWithAutoInvertedBackground(font, sx + sw/2 - tw/2, sy + sh/2 - font.glyph_height/2, valStr, spacing, 1, baseColor, TEXT_PADDING, TEXT_BORDER);

    return changed ? 1 : 0;
}
