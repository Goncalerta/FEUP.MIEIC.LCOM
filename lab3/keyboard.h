#ifndef __KEYBOARD_H
#define __KEYBOARD_H

int (keyboard_subscribe_int)(uint8_t *bit_no); // see lab2; use IRQ_EXCLUSIVE

int (keyboard_unsubscribe_int)();

#endif /* __KEYBOARD_H */