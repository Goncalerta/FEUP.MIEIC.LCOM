#ifndef __BUTTON_H
#define __BUTTON_H

#include <lcom/lcf.h>
#include "graphics.h"

/** @defgroup button button
 * @{
 *
 * @brief Module dedicated to buttons.
 */

#define BUTTON_CIRCLE_RADIUS_DEFAULT 15 /**< @brief Button circle default radius */
#define BUTTON_CIRCLE_DEFAULT_COLOR 0x000000 /**< @brief Button circle default color */

typedef int (*button_action)(); /**< @brief Button action when it's pressed. */

/**
 * @brief Enumerated type for specifying the state of a button.
 * 
 */
typedef enum button_state_t {
    BUTTON_NORMAL, /*!< Button is in normal/base state. */
    BUTTON_HOVERING, /*!< Button is not being pressed but the cursor is hovering it. */
    BUTTON_PRESSING, /*!< Button is being pressed and the cursor is hovering it. */
    BUTTON_PRESSING_NOT_HOVERING /*!< Button is being pressed but the cursor is not hovering it. */
} button_state_t;

/**
 * @brief Icon of a button.
 * 
 */
typedef struct button_icon_t {
    enum { BUTTON_ICON_NONE, /*!< No icon. */
           BUTTON_ICON_XPM, /*!< xpm image icon. */
           BUTTON_ICON_CIRCLE /*!< Circle icon. */
        } type; /*!< Icon type. */
    union {
        xpm_image_t img; /*!< Button image icon. */
        
        struct {
            uint16_t radius; /*!< Radius of a circle icon. */
            uint32_t color; /*!< Color of a circle icon. */
        } circle; /*!< Button circle icon. */
    } attributes; /*!< Icon attributes. */
} button_icon_t;

/**
 * @brief Button info.
 * 
 */
typedef struct button_t {
    uint16_t x; /*!< Left most x coordinate of the button. */
    uint16_t y; /*!< Top most y coordinate of the button. */
    uint16_t width; /*!< Button width. */
    uint16_t height; /*!< Button height. */
    button_state_t state; /*!< State of the button. */
    button_action action; /*!< Action to perform when button is pressed. */
    button_icon_t icon; /*!< Button icon. */
    bool active_border; /*!< True if the border of the button is active and false otherwise. */
} button_t;

/**
 * @brief Initiates the info of a new button.
 * 
 * @param button address of memory of the button to be initialized
 * @param x button x coordinate
 * @param y button y coordinate
 * @param width button width
 * @param height button height
 * @param action button action
 * @return Return 0 upon success and non-zero otherwise
 */
int new_button(button_t *button, uint16_t x, uint16_t y, uint16_t width, uint16_t height, button_action action);

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
bool button_is_hovering(button_t button, uint16_t x, uint16_t y);

/**
 * @brief Draws a given button to the given buffer.
 * 
 * @param buf buffer
 * @param button button
 * @return Return 0 upon success and non-zero otherwise
 */
int button_draw(frame_buffer_t buf, button_t button);

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
