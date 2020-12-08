#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>

typedef enum kbd_key { // this way we can add more actions if needed
    NO_KEY,
    CHAR,         // when a key of a letter or number is pressed
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

// TODO maybe kbd_event_t or something along those lines would be a better name
typedef struct kbd_state {
    kbd_key key;
    char char_key; // to use when (key == CHAR)
} kbd_state;

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

bool kbd_is_make_code(uint8_t scancode);

bool kbd_scancode_ready();

int kbd_handle_scancode(kbd_state *kbd_state);

int kbd_enable_interrupts();

bool kbd_is_ctrl_pressed();

#endif /* __KEYBOARD_H */
