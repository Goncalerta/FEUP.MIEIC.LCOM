#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include <lcom/lcf.h>

// vg_init, vg_draw_hline and vg_draw_rectangle are already included by lcf
int vg_draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step);
int vg_draw_img(xpm_image_t img, uint16_t x, uint16_t y);
int vg_flip_page();
int vg_clear();

#endif /* __VIDEO_GR_H */
