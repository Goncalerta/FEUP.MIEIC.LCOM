#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"

uint32_t cnt = 0; // sys_inb number of calls

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int ipc_status;
  message msg;
  uint8_t bit_no = KEYBOARD_IRQ;
  int r;
  bool make;
  uint8_t bytes[] = {FIRST_BYTE_TWO_BYTE_SCANCODE, 0}; // some initilization so that bytes[0] != ESC_BREAK_CODE
  bool reading_2nd_byte = false;
  uint8_t size;

  if (keyboard_subscribe_int(&bit_no))
    return 1;
  
  while( bytes[0] != ESC_BREAK_CODE ) { /* You may want to use a different condition */
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */				
          if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
            kbc_ih();
            
            if (reading_2nd_byte) {
              bytes[1] = scancode;
              make = (scancode & BREAKCODE_BIT) == 0;
              size = 2;
              reading_2nd_byte = false;
              if(kbd_print_scancode(make, size, bytes))
                return 1;
            } else {
              switch (scancode)
              {
              case FIRST_BYTE_TWO_BYTE_SCANCODE:
                reading_2nd_byte = true;
                bytes[0] = FIRST_BYTE_TWO_BYTE_SCANCODE;
                break;
              
              default: // 1 byte scancode
                bytes[0] = scancode;
                make = (scancode & BREAKCODE_BIT) == 0;
                size = 1;
                if(kbd_print_scancode(make, size, bytes))
                  return 1;
                break;
              }
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */	
      }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
  }
  return kbd_print_no_sysinb(cnt) && keyboard_unsubscribe_int();
}

int(kbd_test_poll)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
