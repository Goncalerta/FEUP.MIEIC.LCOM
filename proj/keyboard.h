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

typedef struct kbd_event_t {
    kbd_key key;
    char char_key; // to use when (key == CHAR)
    bool is_ctrl_pressed; // easier to send the "packet" when the serial port is implemented (using 2 KBD's)
} kbd_event_t;

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

bool kbd_is_make_code(uint8_t scancode);

//bool kbd_scancode_ready(); // outdated for now

int kbd_handle_scancode(kbd_event_t *kbd_state);

int kbd_enable_interrupts();

#endif /* __KEYBOARD_H */
