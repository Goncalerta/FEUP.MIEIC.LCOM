#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>

/** @defgroup keyboard keyboard
 * @{
 *
 *  Module to interact with the PC's Keyboard.
 */

/**
 * @brief Enumerated type for specifying the type of key that was pressed in the Keyboard.
 */
typedef enum kbd_key { // TODO é suposto documentar todos os "estados"? para aparecer tudo "bonitinho"...
    NO_KEY,
    CHAR, // key of a letter or number is pressed
    CTRL,
    ENTER,
    BACK_SPACE,
    ESC,
    DEL,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT
} kbd_key;

/**
 * @brief Keyboard event (type of key + character (if applicable) + CTRL is either pressed or not)
 */
typedef struct kbd_event_t {
    kbd_key key;
    char char_key; // to use when (key == CHAR)
    bool is_ctrl_pressed;
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
int kbd_enable_interrupts(); // TODO this is not used in the project, delete?

/**@}*/

// TODO outdated for now, might be used if the IH gets modified
///**
// * @brief Checks if a complete scancode is ready to be processed.
// *
// * @return Return true if a scancode is ready and false otherwise
// */
//bool kbd_is_scancode_ready(); 

#endif /* __KEYBOARD_H */
