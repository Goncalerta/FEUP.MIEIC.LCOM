#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include <lcom/lcf.h>
#include "graphics.h"

// vg_init, vg_draw_hline and vg_draw_rectangle are already included by lcf
video_buffer_t vg_get_back_buffer();
uint16_t vg_get_hres();
uint16_t vg_get_vres();
uint16_t vg_get_bytes_per_pixel();
int vg_flip_page();
int vg_clear();

#endif /* __VIDEO_GR_H */
