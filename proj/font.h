#ifndef __FONT_H
#define __FONT_H

#include <lcom/lcf.h>

int load_font(enum xpm_image_type type);

int draw_char(char c, uint16_t x, uint16_t y);

int draw_string(char string[], uint16_t x, uint16_t y);

#endif /* __FONT_H */
