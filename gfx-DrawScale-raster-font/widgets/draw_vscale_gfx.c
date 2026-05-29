/* draw_vscale_gfx.c */
#include "draw_vscale_gfx.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>

void DrawVerticalScale_GFX(int channel, float scale, float offset_y,
                           int x, int y, int w, int h,
                           RasterFont font, uint32_t color) {
    gfx_set_color_uint32(color);

    int spacing = 2;
    int x_end = x + w;
    float centerY = y + h / 2.0f;
    float margin = font.glyph_height;
    
    float y_min = y + margin;
    float y_max = y + h - margin;
    
    float minVal = offset_y - ((h / 2.0f - margin) / scale);
    float maxVal = offset_y + ((h / 2.0f - margin) / scale);

    float min_label_spacing_px = font.glyph_height * 2.5f;
    float step = 50.0f / scale;

    // Основна вертикальна лінія
    gfx_line(x_end, y, x_end, y + h);

    for (float val = ((int)(minVal / step)) * step; val <= maxVal; val += step) {
        // Іверсія Y: числа зростають знизу вгору (зменшення піксельної координати)
        float py = centerY - (val - offset_y) * scale;

        if (py < y_min || py > y_max) continue;

        char label[16];
        snprintf(label, sizeof(label), "%.0f", val);
        
        int chars = utf8_strlen(label);
        int text_width = chars * font.glyph_width;

        // Риска праворуч від лінії
        gfx_line(x_end, (int)py, x_end + 10, (int)py);

        // Текст праворуч від риски
        DrawTextScaled(font,
                       x_end + 25 + spacing - text_width / 2, 
                       (int)(py - font.glyph_height / 2),
                       label, spacing, 1, color);
    }
}
