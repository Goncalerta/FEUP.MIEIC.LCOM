#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>

/** @file 
 * @brief File dedicated to interaction with the PC's Keyboard.
 */

/** @defgroup keyboard keyboard
 * @{
 *
 * @brief Module to interact with the PC's Keyboard.
 */

/**
 * @brief Enumerated type for specifying the type of key that was pressed in the Keyboard.
 */
typedef enum kbd_key {
    NO_KEY, /*!< No key was pressed. May happen when a key is released. */
    CHAR, /*!< The key corresponding to a character recognized by the program (letter, number, space) was pressed. */
    CTRL, /*!< Ctrl key was pressed. */
    ENTER, /*!< Enter key was pressed. */
    BACK_SPACE, /*!< Backspace key was pressed. */
    ESC, /*!< Esc key was pressed. */
    DEL, /*!< Delete key was pressed. */
    ARROW_UP, /*!< Up arrow key was pressed. */
    ARROW_DOWN, /*!< Down arrow key was pressed. */
    ARROW_LEFT, /*!< Left arrow key was pressed. */
    ARROW_RIGHT /*!< Right arrow key was pressed. */
} kbd_key;

/**
 * @brief Keyboard event.
 */
typedef struct kbd_event_t {
    kbd_key key; /*!< @brief Key that was pressed. */
    char char_key; /*!< @brief Char pressed when key == CHAR. */
    bool is_ctrl_pressed; /*!< @brief Ctrl key is pressed. */
} kbd_event_t;

/**
 * @brief Subscribes Keyboard interrupts.
 * 
 * @param bit_no address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes Keyboard interrupts.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int();

/**
 * @brief Checks if a given scancode is a make code.
 * 
 * @param scancode scancode to check
 * @return Return true if the scancode is a make code and false otherwise
 */
bool kbd_is_make_code(uint8_t scancode);

/**
 * @brief @brief Transforms a scancode into organized information about the keys pressed in the form of kbd_event_t.
 * 
 * @param kbd_state address of memory to be initialized with the keyboard state after processing the current scancode
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_handle_scancode(kbd_event_t *kbd_state);

/**
 * @brief Enables keyboard interrupts.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_enable_interrupts();

/**
 * @brief Checks if a complete scancode is ready to be processed.
 *
 * @return Return true if a scancode is ready and false otherwise
*/
bool kbd_is_scancode_ready(); 

/**@}*/

#endif /* __KEYBOARD_H */
