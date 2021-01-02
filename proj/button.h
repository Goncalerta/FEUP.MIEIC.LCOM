#ifndef __BUTTON_H
#define __BUTTON_H

#include <lcom/lcf.h>
#include "graphics.h"

/** @file 
 * @brief File dedicated to buttons.
 */

/** @defgroup button button
 * @{
 *
 * @brief Module dedicated to buttons.
 */

#define BUTTON_CIRCLE_RADIUS_DEFAULT 15 /**< @brief Button circle default radius when the icon is a circle */
#define BUTTON_CIRCLE_DEFAULT_COLOR 0x000000 /**< @brief Button circle default color when the icon is a circle */

typedef int (*button_action_t)(); /**< @brief Button action callback when it's pressed. */

struct button_t;

/**
 * @brief Button class.
 * 
 */
typedef struct button button_t;

/**
 * @brief Initializes a new button.
 * 
 * @param x button x coordinate
 * @param y button y coordinate
 * @param width button width
 * @param height button height
 * @param action button action
 * @return Address of memory of the button initialized, or NULL if an error occurred.
 */
button_t *new_button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, button_action_t action);

/**
 * @brief Initializes a button.
 * 
 * @param button address of memory of the button to be deleted
 */
void delete_button(button_t *button);

/**
 * @brief Sets a xpm image icon to a given button.
 * 
 * @param button address of memory of the button
 * @param icon xpm image icon
 */
void button_set_xpm_icon(button_t *button, xpm_image_t icon);

/**
 * @brief Sets a circle icon to a given button.
 * 
 * @param button address of memory of the button
 * @param radius circle icon radius
 * @param color circle icon color
 */
void button_set_circle_icon(button_t *button, uint16_t radius, uint32_t color);

/**
 * @brief Sets the border of a given button active.
 * 
 * @param button address of memory of the button
 */
void button_set_border_active(button_t *button);

/**
 * @brief Sets the border of a given button inactive.
 * 
 * @param button address of memory of the button
 */
void button_unset_border_active(button_t *button);

/**
 * @brief Checks if the given coordinates are inside the limits of a given button.
 * 
 * @param button button
 * @param x x coordinate
 * @param y y coordinate
 * @return Return true if the coordinates are inside the button and false otherwise
 */
bool button_is_hovering(button_t *button, uint16_t x, uint16_t y);

/**
 * @brief Draws a given button to the given buffer.
 * 
 * @param buf buffer
 * @param button button
 * @return Return 0 upon success and non-zero otherwise
 */
int button_draw(frame_buffer_t buf, button_t *button);

/**
 * @brief Updates the state of a given button according to given mouse info.
 * 
 * @param button address of memory of the button
 * @param hovering button is being hovered
 * @param lb is left button pressed
 * @param rb is right button pressed
 * @return Return 0 upon success and non-zero otherwise
 */
int button_update_state(button_t *button, bool hovering, bool lb, bool rb);

/**@}*/

#endif /* __BUTTON_H */
