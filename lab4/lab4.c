#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "i8254.h"
#include "kbc.h"
#include "mouse.h"

extern int interrupt_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    int ipc_status;
    message msg;
    uint8_t bit_no = MOUSE_IRQ;
    int r;
    int fail = 0;
    int packet_part = 0;
    uint8_t raw_bytes[3] = {0, 0, 0};
    struct packet pp;

    if (mouse_enable_dr())
        return 1; 

    if (mouse_subscribe_int(&bit_no))
        return 1;

    while( cnt > 0 ) { 
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
                    mouse_ih();
                    if (ih_return == 1) {
                        fail = 1;
                        break;
                    } else if (ih_return == 2) {
                        continue;
                    }

                    switch (packet_part) {
                    case 0:
                        if(!mouse_is_valid_first_byte_packet(packet_byte)) continue;
                        raw_bytes[0] = packet_byte;
                        packet_part++;
                        break;
                    case 1:
                        raw_bytes[1] = packet_byte;
                        packet_part++;
                        break;
                    case 2:
                        raw_bytes[2] = packet_byte;
                        mouse_parse_packet(raw_bytes, &pp);
                        mouse_print_packet(&pp);
                        packet_part = 0;
                        cnt--;
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

    if (mouse_unsubscribe_int()) return 1;
    
    if (mouse_disable_dr()) return 1; 

    return fail;
}
// TODO "If the device is in Stream mode (the default) and has been enabled with an Enable (0xF4) command, then the host should disable the device with a Disable (0xF5) command before sending any other command." Synaptics TouchPad Interfacing Guide, pg. 33

// TODO “When the host gets an 0xFE response, it should retry the offending command. If an argument byte elicits an 0xFE response, the host should retransmit the entire command, not just the argument byte.” Synaptics TouhcPad Interfacing Guide, pg. 31

int (mouse_test_async)(uint8_t idle_time) {
    int ipc_status;
    message msg;
    uint8_t bit_no_mouse = MOUSE_IRQ;
    uint8_t bit_no_timer = TIMER0_IRQ;
    int r;
    int fail = 0;
    int packet_part = 0;
    uint8_t raw_bytes[3] = {0, 0, 0};
    struct packet pp;
    int timer_frequency = sys_hz();

    if (timer_subscribe_int(&bit_no_timer))
        return 1;
    
    if (mouse_enable_dr())
        return 1; 

    if (mouse_subscribe_int(&bit_no_mouse))
        return 1;

    while( interrupt_counter/timer_frequency < idle_time) { 
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & BIT(bit_no_mouse)) { /* subscribed interrupt */
                    mouse_ih();
                    if (ih_return == 1) {
                        fail = 1;
                        break;
                    } else if (ih_return == 2) {
                        continue;
                    }
                    interrupt_counter = 0;
                    switch (packet_part) {
                    case 0:
                        if(!mouse_is_valid_first_byte_packet(packet_byte)) continue;
                        raw_bytes[0] = packet_byte;
                        packet_part++;
                        break;
                    case 1:
                        raw_bytes[1] = packet_byte;
                        packet_part++;
                        break;
                    case 2:
                        raw_bytes[2] = packet_byte;
                        mouse_parse_packet(raw_bytes, &pp);
                        mouse_print_packet(&pp);
                        packet_part = 0;
                        break;
                    }   
                }
                if (msg.m_notify.interrupts & BIT(bit_no_timer)) {
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

    if (mouse_unsubscribe_int()) return 1;
    
    if (mouse_disable_dr()) return 1; 

    if (timer_unsubscribe_int()) return 1;

    return fail;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* To be completed */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
