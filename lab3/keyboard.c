#include <lcom/lcf.h>
#include "keyboard.h"
#include "i8042.h"

int hook_id;
uint8_t scancode;

int (keyboard_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no;
  return sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int (keyboard_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void (kbc_ih)() {
  kbc_read_data(&scancode);
}

int (kbc_issue_command)(uint8_t cmd) {
  uint8_t stat;

  while (1) {
    if(util_sys_inb(KBC_ST_REG, &stat)) /*assuming it returns OK*/ //TODO should we really assume?
      return 1;
    /*loop while 8042 input buffer is not empty*/
    if( (stat & KBC_ST_IBF) == 0 ) {
      return sys_outb(KBC_CMD_REG, cmd); /*no args command*/
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int (kbc_read_data)(uint8_t *data) {
  uint8_t stat;

  while(1) {
    if(util_sys_inb(KBC_ST_REG, &stat)) /*assuming it returns OK*/ //TODO should we really assume?
      return 1;
    /*loop while 8042 output buffer is empty*/
    if( stat & KBC_OBF ) {
      if(util_sys_inb(KBC_OUT_BUF, data)) /*ass. it returns OK*/ //TODO should we really assume?
        return 1;
      return (stat & (KBC_PAR_ERR | KBC_TO_ERR)) == 0;
    }
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
  }
  return 1;
}
