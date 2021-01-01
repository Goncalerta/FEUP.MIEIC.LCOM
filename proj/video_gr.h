#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include <lcom/lcf.h>
#include "graphics.h"

// Note: vg_init is already included by lcf.

/** @file 
 * @brief File dedicated to the interaction with the video card.
 */

/** @defgroup video_gr video_gr
 * @{
 *
 * @brief Module dedicated to the interaction with the video card.
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
 * @return Return the horizontal resolution in pixels
 */
uint16_t vg_get_hres();

/**
 * @brief Gets the vertical resolution of the buffer.
 * 
 * @return Return the vertical resolution in pixels
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

/**@}*/

#endif /* __VIDEO_GR_H */
