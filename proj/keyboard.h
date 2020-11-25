#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>

int kbd_ih_return;

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

bool kbd_is_make_code(uint8_t scancode);

bool kbd_scancode_ready();

int kbd_handle_scancode();

int kbd_enable_interrupts();

#endif /* __KEYBOARD_H */
