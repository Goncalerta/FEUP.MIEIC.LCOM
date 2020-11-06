#ifndef __KBC_H
#define __KBC_H

uint8_t scancode;
int ih_return;

int (kbc_write_reg)(uint8_t r, uint8_t value);

int (kbc_issue_cmd)(uint8_t cmd);

int (kbc_pass_cmd_arg)(uint8_t argument_byte);

int (kbc_read_data)(uint8_t *data, int mouse_data);

int (kbc_read_byte_command)(uint8_t *command_byte);

int (kbc_write_byte_command)(uint8_t command_byte);

#endif /* __KBC_H */
