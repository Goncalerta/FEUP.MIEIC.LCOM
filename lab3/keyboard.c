#include <lcom/lcf.h>
#include "keyboard.h"
#include "i8042.h"

int (keyboard_subscribe_int)(uint8_t *bit_no) {
  hookId = *bit_no;
  return sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookId);
}

int (keyboard_unsubscribe_int)() {
  return sys_irqrmpolicy(&hookId);
}
