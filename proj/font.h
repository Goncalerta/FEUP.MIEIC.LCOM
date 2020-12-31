#ifndef __FONT_H
#define __FONT_H

#include <lcom/lcf.h>
#include "graphics.h"

/** @defgroup font font
 * @{
 *
 * Module dedicated to the font used.
 */

#define FONT_CHAR_WIDTH 18 /**< @brief Font char width */
#define FONT_CHAR_HEIGHT 18 /**< @brief Font char height */

#define CHAR_SPACE (FONT_CHAR_WIDTH + 2) /**< @brief Width of a char plus the space between 2 chars */

/**
 * @brief Loads the font from a .xpm to the font module.
 * 
 * @param type type of xpm image
 * @return Return 0 upon success and non-zero otherwise
 */
int font_load(enum xpm_image_type type);

/**
 * @brief Frees the space allocated in memory to store the font.
 * 
 */
void font_unload();

/**
 * @brief Draws a given char to the given coordinates in the given buffer.
 * 
 * @param buf buffer
 * @param c char to draw
 * @param x x coordinate
 * @param y y coordinate
 * @return Return 0 upon success and non-zero otherwise
 */
int font_draw_char(frame_buffer_t buf, char c, uint16_t x, uint16_t y);

/**
 * @brief Draws a given string to the given coordinates in the given buffer.
 * 
 * @param buf buffer
 * @param string string to draw
 * @param x x coordinate
 * @param y y coordinate
 * @return Return 0 upon success and non-zero otherwise
 */
int font_draw_string(frame_buffer_t buf, const char string[], uint16_t x, uint16_t y);

/**
 * @brief Draws a substring of a string to the given coordinates in the given buffer.
 * 
 * @param buf buffer
 * @param string string which contains the substring
 * @param x x coordinate
 * @param y y coordinate
 * @param start substring start
 * @param size substring size
 * @return Return 0 upon success and non-zero otherwise
 */
int font_draw_string_limited(frame_buffer_t buf, const char string[], uint16_t x, uint16_t y, uint8_t start, uint8_t size);

/**
 * @brief Draws a substring of a string centered in the given coordinates in the given buffer.
 * 
 * @param buf buffer
 * @param string string which contains the substring
 * @param x x coordinate
 * @param y y coordinate
 * @param start substring start
 * @param size substring size
 * @return Return 0 upon success and non-zero otherwise
 */
int font_draw_string_centered(frame_buffer_t buf, const char string[], uint16_t x, uint16_t y, uint8_t start, uint8_t size);

/**@}*/

#endif /* __FONT_H */
