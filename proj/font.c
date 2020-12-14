#include <lcom/lcf.h>
#include "font.h"
#include "xpm/font.xpm"
#include "graphics.h"
#include "video_gr.h"

static xpm_image_t font;

int font_load(enum xpm_image_type type) {
    if (xpm_load(xpm_font, type, &font) == NULL) {
        printf("Erro loading font\n");
        return 1;
    }
    return 0;
}

int font_draw_char(frame_buffer_t buf, char c, uint16_t x, uint16_t y) {
    uint16_t char_start_x = 0;
    uint16_t char_start_y = 0;
    
    if ('0' <= c && c <= '9') {
        char_start_y = 2*FONT_CHAR_SPACE_Y + FONT_CHAR_HEIGHT;
        char_start_x = (c-'0'+1)*FONT_CHAR_SPACE_X + (c-'0')*FONT_CHAR_WIDTH;
        
    } else if ('A' <= c && c <= 'Z') {
    	char_start_y = FONT_CHAR_SPACE_Y;
    	char_start_x = (c-'A'+1)*FONT_CHAR_SPACE_X + (c-'A')*FONT_CHAR_WIDTH;
    	
    } else if (c == '.') {
        // TODO maybe include this in font xpm?
        vb_draw_rectangle(buf, x + (FONT_CHAR_WIDTH - 5) / 2, y + FONT_CHAR_HEIGHT - 5, 5, 5, 0x000000);
    	return 0;
        
    } else if (c == ' ') {
        return 0; // no need to draw space

    } else {
    	printf("Invalid char to draw: %c\n", c);
    	return 1;
    }
    
    if (vb_draw_img(buf, font, char_start_x, char_start_y, FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT, x, y) != 0) {
        return 1;
    }

    return 0;
}

int font_draw_string(frame_buffer_t buf, char string[], uint16_t x, uint16_t y, uint8_t start, uint8_t size) {
    for (int i = 0; string[i] != '\0' && i < start + size; i++) {
        if (i < start) {
            continue;
        }

        if (font_draw_char(buf, string[i], x + (i-start)*CHAR_SPACE, y) != 0) {
            printf("font_draw_char error\n");
            return 1;
        }
    }
    return 0;
}
