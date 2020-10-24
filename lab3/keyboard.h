#ifndef __KEYBOARD_H
#define __KEYBOARD_H

uint8_t scancode;

int (keyboard_subscribe_int)(uint8_t *bit_no); // see lab2; use IRQ_EXCLUSIVE

int (keyboard_unsubscribe_int)();

void (kbc_ih)();

int (kbc_issue_command)(uint8_t cmd);

int (kbc_read_data)(uint8_t *data);

#endif /* __KEYBOARD_H */
