#ifndef __KBC_H
#define __KBC_H

#include <lcom/lcf.h>
#include "i8042.h"

int kbc_write_reg(int reg, uint8_t write);

int kbc_issue_command(uint8_t cmd);

int kbc_issue_argument(uint8_t arg);

int kbc_read_data(uint8_t *data);

int kbc_read_command_byte(uint8_t *byte);

int kbc_write_command_byte(uint8_t byte);

#endif /* __KBC_H */
