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
  uint8_t bytes[] = {0, 0}; // some initilization so that bytes[0] != ESC_BREAK_CODE
  bool reading_2nd_byte = false;
  int fail = 0;

  if (keyboard_subscribe_int(&bit_no))
    return 1;
  
  while( bytes[0] != ESC_BREAK_CODE ) {
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
            if (ih_return) {
              fail = 1;
              break;
            }
            
            if (reading_2nd_byte) {
              bytes[1] = scancode;
              bool make = (scancode & BREAKCODE_BIT) == 0;
              uint8_t size = 2;
              reading_2nd_byte = false;
              if(kbd_print_scancode(make, size, bytes)) {
                fail = 1;
                break;
              }
            } else {
              bytes[0] = scancode;
              if (scancode == FIRST_BYTE_TWO_BYTE_SCANCODE) {
                reading_2nd_byte = true;
              } else {
                bool make = (scancode & BREAKCODE_BIT) == 0;
                uint8_t size = 1;
                if(kbd_print_scancode(make, size, bytes)) {
                  fail = 1;
                  break;
                }
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

  return keyboard_unsubscribe_int() || fail || kbd_print_no_sysinb(cnt);
}

int(kbd_test_poll)() {
  uint8_t bytes[] = {0, 0}; // some initilization so that bytes[0] != ESC_BREAK_CODE
  bool reading_2nd_byte = false;
  int fail = 0;

  while( bytes[0] != ESC_BREAK_CODE ) {
    if (kbc_read_data(&scancode)) {
      fail = 1;
      break;
    }
    
    if (reading_2nd_byte) {
      bytes[1] = scancode;
      bool make = (scancode & BREAKCODE_BIT) == 0;
      uint8_t size = 2;
      reading_2nd_byte = false;
      if(kbd_print_scancode(make, size, bytes)) {
        fail = 1;
        break;
      }
    } else {
      bytes[0] = scancode;
      if (scancode == FIRST_BYTE_TWO_BYTE_SCANCODE) {
        reading_2nd_byte = true;
      } else {
        bool make = (scancode & BREAKCODE_BIT) == 0;
        uint8_t size = 1;
        if(kbd_print_scancode(make, size, bytes)) {
          fail = 1;
          break;
        }
      }
    } 
  }

  uint8_t command_byte;
  if (kbc_read_byte_command(&command_byte))
    return 1;
  
  command_byte |= CMD_BYTE_ENABLE_KBD_INT;
  if (kbc_write_byte_command(command_byte))
    return 1;

  return fail || kbd_print_no_sysinb(cnt);
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
