#ifndef __FONT_H
#define __FONT_H

#include <lcom/lcf.h>
#include "graphics.h"

#define FONT_CHAR_WIDTH 18
#define FONT_CHAR_HEIGHT 18
#define FONT_CHAR_SPACE_X 2
#define FONT_CHAR_SPACE_Y 4

#define CHAR_SPACE (FONT_CHAR_WIDTH + 2) // talvez precise de ajustes

int font_load(enum xpm_image_type type);

int font_draw_char(frame_buffer_t buf, char c, uint16_t x, uint16_t y);

// TODO wrapper function to call when we need to draw the whole string
int font_draw_string(frame_buffer_t buf, char string[], uint16_t x, uint16_t y, uint8_t start, uint8_t size);

#endif /* __FONT_H */
