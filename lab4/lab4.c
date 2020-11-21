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
    int ipc_status, r;
    message msg;
    uint8_t bit_no;
    int fail = 0;

    int packet_part = 0;
    uint8_t raw_bytes[3] = {0, 0, 0};
    struct packet pp;

    if (mouse_enable_dr() != OK)
        return 1; 

    if (mouse_subscribe_int(&bit_no) != OK)
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
                    if (mouse_ih_return == 1) {
                        fail = 1;
                        break;
                    } else if (mouse_ih_return == 2) {
                        continue;
                    }

                    switch (packet_part) {
                    case 0:
                        if(!mouse_is_valid_first_byte_packet(packet_byte)) 
                            continue;
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

    if (mouse_unsubscribe_int() != OK) 
        return 1;
    
    if (mouse_disable_dr() != OK) 
        return 1; 

    return fail;
}

int (mouse_test_async)(uint8_t idle_time) {
    int ipc_status, r;
    message msg;
    uint8_t bit_no_mouse, bit_no_timer;
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
                    if (mouse_ih_return == 1) {
                        fail = 1;
                        break;
                    } else if (mouse_ih_return == 2) {
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

    if (mouse_unsubscribe_int() != OK) 
        return 1;
    
    if (mouse_disable_dr() != OK) 
        return 1; 

    if (timer_unsubscribe_int() != OK) 
        return 1;

    return fail;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    int ipc_status, r;
    message msg;
    uint8_t bit_no;
    int fail = 0;

    int packet_part = 0;
    uint8_t raw_bytes[3] = {0, 0, 0};
    struct packet pp;
    enum mouse_gesture_state gesture_state = STATE_BEGIN;
    uint8_t x_displ;

    if (mouse_enable_dr() != OK)
        return 1; 

    if (mouse_subscribe_int(&bit_no) != OK)
        return 1;

    while( gesture_state != STATE_RB_RELEASED ) { 
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
                    if (mouse_ih_return == 1) {
                        fail = 1;
                        break;
                    } else if (mouse_ih_return == 2) {
                        continue;
                    }

                    switch (packet_part) {
                    case 0:
                        if(!mouse_is_valid_first_byte_packet(packet_byte)) 
                            continue;
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
                        packet_part = 0;
                        mouse_print_packet(&pp);
                        mouse_update_gesture_state(x_len, tolerance, pp, &gesture_state, &x_displ);
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

    if (mouse_unsubscribe_int() != OK) 
        return 1;
    
    if (mouse_disable_dr() != OK) 
        return 1; 

    return fail;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    int packet_part = 0;
    uint8_t raw_bytes[3] = {0, 0, 0};
    struct packet pp;
    bool fail = false;

    while( cnt > 0 ) { 
        if (write_byte_to_mouse(MS_READ_DATA) != OK)
            return 1;
        int result = kbc_read_data(&packet_byte, true);
        if (result == 1) {
            fail = 1;
            break;
        } else if (result == 2) {
            continue;
        }

        switch (packet_part) {
        case 0:
            if(!mouse_is_valid_first_byte_packet(packet_byte)) 
                continue;
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
        tickdelay(micros_to_ticks(period));
    }

    if (mouse_disable_dr() != OK) 
        return 1;
        
    if (mouse_set_stream_mode() != OK)
        return 1;

    if (mouse_disable_dr() != OK) 
        return 1; 
    
    if (kbc_write_command_byte(minix_get_dflt_kbc_cmd_byte()) != OK)
        return 1;

    return fail;
}
