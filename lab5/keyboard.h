#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>

uint8_t scancode;
int kbc_ih_return;

int (kbd_subscribe_int)(uint8_t *bit_no);

int (kbd_unsubscribe_int)();

bool (is_make_code)(uint8_t scancode);

int (kbd_enable_interrupts)();

int (kbd_update_scancode)(uint8_t scancode, uint8_t *size, uint8_t *bytes);

int (kbd_display_scancode)(uint8_t scancode, uint8_t *size, uint8_t *bytes);

#endif /* __KEYBOARD_H */
