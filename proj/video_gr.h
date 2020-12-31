#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include <lcom/lcf.h>
#include "graphics.h"

// TODO vg_draw_hline and vg_draw_rectangle were renamed to vb_draw_hline and vb_draw_rectangle, let it be that way?
// vg_init, vg_draw_hline and vg_draw_rectangle are already included by lcf

/** @defgroup video_gr video_gr
 * @{
 *
 * Module dedicated to the video graphics.
 */

/**
 * @brief Gets the back buffer.
 * 
 * @return Return the back buffer
 */
frame_buffer_t vg_get_back_buffer();

/**
 * @brief Gets the horizontal resolution of the buffer.
 * 
 * @return Return the horizontal resolution
 */
uint16_t vg_get_hres();

/**
 * @brief Gets the vertical resolution of the buffer.
 * 
 * @return Return the vertical resolution
 */
uint16_t vg_get_vres();

/**
 * @brief Gets the number of bytes used to represent a single pixel.
 * 
 * @return Return the number of bytes
 */
uint16_t vg_get_bytes_per_pixel();

/**
 * @brief Flips the front buffer with the back buffer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int vg_flip_page();

/**
 * @brief Paints the back buffer all white.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int vg_clear();

/**@}*/

#endif /* __VIDEO_GR_H */
