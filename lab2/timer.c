#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  int read_back_command = TIMER_RB_CMD | TIMER_RB_STATUS_ | TIMER_RB_SEL(timer);
  int fail = sys_outb(TIMER_CTRL, read_back_command);
  if(fail) return fail;

  return util_sys_inb(TIMER(timer), st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val conf;
  switch (field) {
    case tsf_all: 
      conf.byte = st; 
      break;
    case tsf_initial: 
      switch ((st & MASK_INITIAL) >> 4) {
        case 1:
          conf.in_mode = LSB_only;
          break;
        case 2:
          conf.in_mode = MSB_only;
          break;
        case 3:
          conf.in_mode = MSB_after_LSB;
          break;
        default:
          conf.in_mode = INVAL_val;
          break;
      }
      break;
    case tsf_mode: 
      conf.count_mode = (uint8_t) ((st & MASK_MODE) >> 1); 
      break;
    case tsf_base: 
      conf.bcd = (bool) (st & MASK_BASE); 
      break;
  }
  timer_print_config(timer, field, conf);

  return 1;
}
