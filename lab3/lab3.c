#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "kbc.h"
#include "i8254.h"

uint32_t cnt = 0; // sys_inb number of calls
int interrupt_counter;

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
    int ipc_status, r;
    message msg;
    uint8_t bit_no;
    bool fail = false;

    uint8_t bytes[] = {0, 0};
    uint8_t size = 1;

    if (kbd_subscribe_int(&bit_no))
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
                    if (kbc_ih_return == 1) {
                        fail = true;
                        break;
                    } else if (kbc_ih_return == 2) {
                        continue;
                    }
                
                    if (kbd_display_scancode(scancode, &size, bytes) != OK) {
                        fail = true;
                        break;
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
    
    if (kbd_unsubscribe_int() != OK) 
        return 1;
    if (fail) 
        return 1;
    if (kbd_print_no_sysinb(cnt) != OK) 
        return 1;

    return 0;
}

int(kbd_test_poll)() {
    uint8_t bytes[] = {0, 0}; // some initilization so that bytes[0] != ESC_BREAK_CODE
    uint8_t size = 1;
    bool fail = false;

    while( bytes[0] != ESC_BREAK_CODE ) {
        uint8_t read_data_return = kbc_read_data(&scancode);
        if (read_data_return == 1) {
            fail = true;
            break;
        } else if (read_data_return == 2) {
            continue;
        }
    
        if (kbd_display_scancode(scancode, &size, bytes) != OK) {
            fail = true;
            break;
        }
    }

    if (kbd_enable_interrupts() != OK) 
        return 1;
    if (fail) 
        return 1;
    if (kbd_print_no_sysinb(cnt) != OK) 
        return 1;

    return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
    int ipc_status, r;
    message msg;
    uint8_t bit_no_kbd, bit_no_timer0;
    bool fail = false;

    uint8_t bytes[] = {0, 0};
    uint8_t size = 1;

    if (timer_subscribe_int(&bit_no_timer0)) 
        return 1;

    if (kbd_subscribe_int(&bit_no_kbd))
        return 1;


    while( bytes[0] != ESC_BREAK_CODE && interrupt_counter / 60 < n) {
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & BIT(bit_no_kbd)) { /* subscribed interrupt */
                    kbc_ih();
                    if (kbc_ih_return == 1) {
                        fail = true;
                        break;
                    } else if (kbc_ih_return == 2) {
                        continue;
                    }
                
                    interrupt_counter = 0; // reset the time interval between scancodes
                    if (kbd_display_scancode(scancode, &size, bytes) != OK) {
                        fail = true;
                        break;
                    }
                }
                if (msg.m_notify.interrupts & BIT(bit_no_timer0)) {
                    timer_int_handler();
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }

    if (kbd_unsubscribe_int() != OK) 
        return 1;
    if (timer_unsubscribe_int() != OK) 
        return 1;

    return fail;
}
