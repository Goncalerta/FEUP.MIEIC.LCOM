#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include <lcom/lcf.h>

int (vg_clear)();
int (vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step);
int (vg_draw_img)(xpm_image_t img, uint16_t x, uint16_t y);
int vg_flip_page();
int vbe_get_contr_info(vg_vbe_contr_info_t *info_p);

#endif /* __VIDEO_GR_H */
