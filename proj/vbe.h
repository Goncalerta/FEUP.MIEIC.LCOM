#ifndef __VBE_H
#define __VBE_H

#include <lcom/lcf.h>

/** @file 
 * @brief VBE functions.
 */

/** @defgroup vbe vbe
 * @{
 *
 * @brief Vbe functions.
 */

#define VBE_LINEAR_FRAME_BUFFER_MODEL BIT(14)  /*!< @brief Linear frame buffer model bit. */
#define VBE_CALL_AH 0x4F /*!< @brief VBE funtion call ah register value. */
#define VBE_FUNCTION_GET_CONTR_INFO 0x00  /*!< @brief VBE GET CONTROLLER INFO function code. */
#define VBE_FUNCTION_RETURN_MODE_INFO 0x01 /*!< @brief VBE RETURN MODE INFO function code. */
#define VBE_FUNCTION_SET_MODE 0x02 /*!< @brief VBE SET MODE INFO function code. */
#define VBE_FUNCTION_SET_GET_DISPLAY_START 0x07 /*!< @brief VBE SET/GET DISPLAY START function code. */
#define VBE_FUNCTION_AH_SUCCESS 0x00 /*!< @brief VBE function ah register success value. */

#define BIOS_VIDEO_SERVICES 0x10 /*!< @brief Software interrupt instruction bios video services code. */

#define SET_DISPLAY_START_DURING_VERTICAL_RETRACE 0x80 /*!< @brief Set display start during vertical retrace bl register value (function 0x07) */

/**
 * @brief VBE function 0x02 SET VBE MODE.
 * 
 * @param mode selected graphic mode
 * @return Return 0 upon success and non-zero otherwise
 */
int vbe_change_mode(uint16_t mode);

/**
 * @brief VBE function 0x07 SET DISPLAY START.
 * 
 * @param first_pixel_in_scanline first pixel in the scanline to display
 * @param first_scanline first scanline to display
 * @return Return 0 upon success and non-zero otherwise
 */
int vbe_set_display_start(uint16_t first_pixel_in_scanline, uint16_t first_scanline);

/**
 * @brief VBE function 0x01 RETURN VBE MODE INFORMATION.
 * 
 * @param mode mode to get the info about
 * @param vmi address of memory to be initialized with the mode info
 * @return Return 0 upon success and non-zero otherwise
 */
int vbe_get_mode_inf(uint16_t mode, vbe_mode_info_t *vmi);

/**@}*/

#endif /* __VBE_H */
