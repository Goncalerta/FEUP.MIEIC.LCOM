#ifndef __KEYBOARD_H
#define __KEYBOARD_H

uint8_t scancode;
int ih_return;

int (keyboard_subscribe_int)(uint8_t *bit_no);

int (keyboard_unsubscribe_int)();

int (kbc_issue_command)(uint8_t cmd);

int (kbc_read_data)(uint8_t *data);

int (kbc_read_byte_command)(uint8_t *command_byte);

int (kbc_write_byte_command)(uint8_t command_byte);

#endif /* __KEYBOARD_H */
