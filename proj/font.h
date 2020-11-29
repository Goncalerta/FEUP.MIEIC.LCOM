#ifndef __FONT_H
#define __FONT_H

#include <lcom/lcf.h>

int font_load(enum xpm_image_type type);

int font_draw_char(char c, uint16_t x, uint16_t y);

int font_draw_string(char string[], uint16_t x, uint16_t y);

#endif /* __FONT_H */
