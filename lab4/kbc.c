#include <lcom/lcf.h>
#include "kbc.h"
#include "i8042.h"

void (kbc_ih)() {
  ih_return = kbc_read_data(&scancode, 0);
}

int (kbc_write_reg)(uint8_t reg, uint8_t value) {
  uint8_t stat;

  for (int i = 0; i < 5; i++) {
    if(util_sys_inb(KBC_ST_REG, &stat)) 
      return 1;
    /*loop while 8042 input buffer is not empty*/
    if( (stat & KBC_ST_IBF) == 0 ) {
      return sys_outb(reg, value); 
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int (kbc_issue_cmd)(uint8_t cmd) {
  return kbc_write_reg(KBC_CMD_REG, cmd);
}

int (kbc_pass_cmd_arg)(uint8_t argument_byte) {
  return kbc_write_reg(KBC_CMD_ARGUMENTS_REG, argument_byte);
}

int (kbc_read_data)(uint8_t *data, int mouse_data) {
  uint8_t stat;

  for (int i = 0; i < 5; i++) {
    if(util_sys_inb(KBC_ST_REG, &stat))
      return 1;

    int read_data_from_mouse = (stat & KBC_AUX) >> 5;
    /*loop while 8042 output buffer is empty*/
    if( (stat & KBC_OBF) && (read_data_from_mouse == mouse_data) ) {
      if(util_sys_inb(KBC_OUT_BUF, data))
        return 1;
      if ((stat & (KBC_PAR_ERR | KBC_TO_ERR)) != 0)
        return 2;
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US)); 
  }
  return 1;
}

int (kbc_read_byte_command)(uint8_t *command_byte) {
  return kbc_issue_cmd(CMD_READ_BYTE) 
      || util_sys_inb(KBC_OUT_BUF, command_byte);
}

int (kbc_write_byte_command)(uint8_t command_byte) {
  return kbc_issue_cmd(CMD_WRITE_BYTE) 
      || sys_outb(KBC_CMD_ARGUMENTS_REG, command_byte);
}
