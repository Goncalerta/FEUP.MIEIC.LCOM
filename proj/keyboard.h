#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>

enum KBD_KEY { // this way we can add more actions if needed
    NO_KEY,
    CHAR,         // when a key of a letter or number is pressed
    CTRL,
    ENTER,
    SPACE,
    BACK_SPACE,
    ESC,
    DEL,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT
};

typedef struct KBD_STATE {
    enum KBD_KEY key;
    char char_key; // to use when (key == CHAR)
} KBD_STATE;

int kbd_ih_return;

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

bool kbd_is_make_code(uint8_t scancode);

bool kbd_scancode_ready();

int kbd_handle_scancode(KBD_STATE *kbd_state);

int kbd_enable_interrupts();

bool kbd_is_ctrl_pressed();

#endif /* __KEYBOARD_H */
