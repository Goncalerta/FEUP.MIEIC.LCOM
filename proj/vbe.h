#ifndef __VBE_H
#define __VBE_H

#include <lcom/lcf.h>

/** @defgroup vbe vbe
 * @{
 *
 * Vbe functions.
 */

/**
 * @brief VBE function 0x02.
 * 
 * @param mode selected graphic mode
 * @return Return 0 upon success and non-zero otherwise
 */
int vbe_change_mode(uint16_t mode);

/**
 * @brief VBE function 0x07.
 * 
 * @param first_pixel_in_scanline first pixel in the scanline to display
 * @param first_scanline first scanline to display
 * @return Return 0 upon success and non-zero otherwise
 */
int vbe_set_display_start(uint16_t first_pixel_in_scanline, uint16_t first_scanline);

/**
 * @brief VBE function 0x01.
 * 
 * @param mode mode to get the info about
 * @param vmi address of memory to be initialized with the mode info
 * @return Return 0 upon success and non-zero otherwise
 */
int vbe_get_mode_inf(uint16_t mode, vbe_mode_info_t *vmi);

/**@}*/

#endif /* __VBE_H */
