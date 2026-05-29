/* draw_hscale_gfx.c */
#include "draw_hscale_gfx.h"
#include "graphics.h"
#include "gfx.h"
#include <stdio.h>

void DrawHorizontalScale_GFX(int channel, float scale, float offset_x,
                             int x, int y, int w, int h,
                             RasterFont font, uint32_t color) {
    gfx_set_color_uint32(color);

    int spacing = 2;
    int y_end = y + h;
    float centerX = x + w / 2.0f;
    float margin = font.glyph_height;
    
    float x_min = x + margin;
    float x_max = x + w - margin;
    
    float minVal = offset_x - ((w / 2.0f - margin) / scale);
    float maxVal = offset_x + ((w / 2.0f - margin) / scale);

    float min_label_spacing_px = font.glyph_height * 2.5f;
    float step = 50.0f / scale;

    // Основна горизонтальна лінія
    gfx_line(x, y_end, x + w, y_end);

    for (float val = ((int)(minVal / step)) * step; val <= maxVal; val += step) {
        float px = centerX + (val - offset_x) * scale;

        if (px < x_min || px > x_max) continue;

        char label[16];
        snprintf(label, sizeof(label), "%.0f", val);
        
        int chars = utf8_strlen(label);
        int text_width = chars * font.glyph_width;

        // Риска зверху від лінії
        gfx_line((int)px, y_end, (int)px, y_end - 10);

        // Текст над рискою
        DrawTextScaled(font,
                       (int)(px - text_width / 2),
                       y_end - font.glyph_height - 15,
                       label, spacing, 1, color);
    }
}
