#include <lcom/lcf.h>
#include "keyboard.h"
#include "i8042.h"

int hook_id;

int (keyboard_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no;
  return sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int (keyboard_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void (kbc_ih)() {
  ih_return = kbc_read_data(&scancode);
}

int (kbc_issue_command)(uint8_t cmd) {
  uint8_t stat;

  while (1) {
      // TODO To make your code resilient to failures in the
      // KBC/keyboard, it should give up after “enough time” for
      // the KBC to send a previous command/data to the KBD.
    if(util_sys_inb(KBC_ST_REG, &stat)) 
      return 1;
    /*loop while 8042 input buffer is not empty*/
    if( (stat & KBC_ST_IBF) == 0 ) {
      return sys_outb(KBC_CMD_REG, cmd); 
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int (kbc_read_data)(uint8_t *data) {
  uint8_t stat;

  while(1) {
      // TODO It is not robust against failures in the KBC/keyboard
    if(util_sys_inb(KBC_ST_REG, &stat))
      return 1;
    /*loop while 8042 output buffer is empty*/
    if( (stat & KBC_OBF) && !(stat & KBC_AUX) ) {
      if(util_sys_inb(KBC_OUT_BUF, data))
        return 1;
      return (stat & (KBC_PAR_ERR | KBC_TO_ERR)) != 0; // TODO should these parity/timeout errors cause stop the loop or just discard the data and continue?
    }
    tickdelay(micros_to_ticks(DELAY_US)); 
  }
  return 1;
}

int (kbc_read_byte_command)(uint8_t *command_byte) {
  return kbc_issue_command(CMD_READ_BYTE) 
      || util_sys_inb(KBC_OUT_BUF, command_byte);
}

int (kbc_write_byte_command)(uint8_t command_byte) {
  return sys_outb(KBC_CMD_ARGUMENTS_REG, command_byte) 
      || kbc_issue_command(CMD_WRITE_BYTE);
}
