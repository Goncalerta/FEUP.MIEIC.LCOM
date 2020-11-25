#ifndef __VBE_H
#define __VBE_H

#include <lcom/lcf.h>

int vbe_change_mode(uint16_t mode);
int vbe_set_display_start(uint16_t first_pixel_in_scanline, uint16_t first_scanline);
int vbe_get_mode_inf(uint16_t mode, vbe_mode_info_t *vmi);

#endif /* __VBE_H */
