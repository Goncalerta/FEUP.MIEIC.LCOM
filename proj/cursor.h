#ifndef _CURSOR_H_
#define _CURSOR_H_

#include <lcom/lcf.h>

/** @file 
 * @brief File dedicated to the mouse cursor.
 */

/** @defgroup cursor cursor
 * @{
 *
 * @brief Module dedicated to the mouse cursor.
 */

/**
 * @brief Enumerated type for specifying the state of the cursor.
 * 
 */
typedef enum cursor_state {
    CURSOR_ARROW, /*!< Cursor is in arrow state. */
    CURSOR_PAINT, /*!< Cursor is in paint state. */
    CURSOR_WRITE, /*!< Cursor is in write state. */
    CURSOR_DISABLED /*!< Cursor is in disabled state. */
} cursor_state;

/**
 * @brief Loads the cursor .xpm images and sets the initial cursor position.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int cursor_init();

/**
 * @brief Frees the space allocated in memory to store the cursor images.
 * 
 */
void cursor_exit();

/**
 * @brief Gets cursor x coordinate
 * 
 * @return Return cursor x coordinate
 */
int16_t cursor_get_x();

/**
 * @brief Gets cursor y coordinate
 * 
 * @return Return cursor y coordinate
 */
int16_t cursor_get_y();

/**
 * @brief Gets the left button state. (either pressed or not)
 * 
 * @return Return true if left button is pressed and false otherwise
 */
bool cursor_get_lb();

/**
 * @brief Gets the right button state. (either pressed or not)
 * 
 * @return Return true if right button is pressed and false otherwise
 */
bool cursor_get_rb();

/**
 * @brief Moves the cursor by the given displacement.
 * 
 * @param dx x displacement
 * @param dy y displacement
 */
void cursor_move(int16_t dx, int16_t dy);

/**
 * @brief Updates the buttons state.
 * 
 * @param new_lb new left botton state
 * @param new_rb new right botton state
 */
void cursor_update_buttons(bool new_lb, bool new_rb);

/**
 * @brief Draws the cursor to the back buffer in its current position according to its current state.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int cursor_draw();

/**
 * @brief Sets the cursor state.
 * 
 * @param state cursor new state
 */
void cursor_set_state(cursor_state state);

/**@}*/

#endif /* _CURSOR_H */
