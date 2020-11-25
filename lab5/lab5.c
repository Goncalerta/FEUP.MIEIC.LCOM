// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "video_gr.h"
#include "defines_graphic.h"
#include "i8042.h"
#include "keyboard.h"
#include "sprite.h"

int interrupt_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
    if (vg_init(mode) == NULL) 
        return 1;
    
    tickdelay(micros_to_ticks(delay * SECONDS_TO_MICROS));

    if (vg_exit() != OK) 
        return 1;
    
    return 0;
}

/*The test of these functions is based essentially on the contents of the frame-buffer upon calling vg_exit(). Therefore, you should invoke VBE function 0x02, Set VBE Mode, with bit 15 of the BX register cleared, thus ensuring that the display memory is cleared after switching to the desired graphics mode.*/
//TODO ^^ a vg_exit() não faz já isto ao dar memset com 0's?
int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
    int ipc_status, r;
    message msg;
    uint8_t bit_no;
    bool fail = false;

    uint8_t bytes[] = {0, 0};
    uint8_t size = 1;

    if (vg_init(mode) == NULL) 
        return 1;

    if (vg_draw_rectangle(x, y, width, height, color) != OK) 
        return 1;
    
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
                
                    if (kbd_update_scancode(scancode, &size, bytes) != OK) {
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

    if (vg_exit() != OK)
        return 1;

    return fail;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
        int ipc_status, r;
    message msg;
    uint8_t bit_no;
    bool fail = false;

    uint8_t bytes[] = {0, 0};
    uint8_t size = 1;

    if (vg_init(mode) == NULL) 
        return 1;

    if (vg_draw_pattern(no_rectangles, first, step) != OK) 
        return 1;
    
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
                
                    if (kbd_update_scancode(scancode, &size, bytes) != OK) {
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

    if (vg_exit() != OK)
        return 1;

    return fail;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    uint16_t mode = 0x105;
    int ipc_status, r;
    message msg;
    uint8_t bit_no;
    bool fail = false;

    uint8_t bytes[] = {0, 0};
    uint8_t size = 1;

    if (vg_init(mode) == NULL) 
        return 1;

    xpm_image_t img;
    if (xpm_load(xpm, XPM_INDEXED, &img) == NULL)
        return 1;

    if (vg_draw_img(img, x, y) != OK)
        return 1;
    
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
                
                    if (kbd_update_scancode(scancode, &size, bytes) != OK) {
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

    if (vg_exit() != OK)
        return 1;

    return fail;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
    uint16_t mode = 0x105;
    int ipc_status, r;
    message msg;
    uint8_t kbd_bit_no, timer_bit_no;
    bool fail = false;

    uint8_t bytes[] = {0, 0};
    uint8_t size = 1;

    uint8_t interrupts_per_frame = sys_hz() / fr_rate;
    if (interrupts_per_frame == 0)
        return 1;

    if (vg_init(mode) == NULL) 
        return 1;

    if (xi != xf && yi != yf)
        return 1; // Only horizontal and vertical movement is allowed

    int xspeed = 0, yspeed = 0, s;

    if (speed >= 0) {
        s = speed;
    } else {
        s = 1; // Negative speeds represent number of frames necessary for displacement of 1 pixel
               // so the speed in each displacement is 1 pixel
    }

    if (xf > xi) { // Going to the right
        xspeed = s;
    } else if (xf < xi) { // Going to the left
        xspeed = -s;
    } else if (yf > yi) { // Going downwards
        yspeed = s;
    } else { // Going upwards
        yspeed = -s;
    }

    Sprite *sprite = create_sprite(xpm, xi, yi, xspeed, yspeed);
    if (sprite == NULL)
        return 1;

    if (draw_sprite(sprite) != OK)
        return 1;

    if (timer_subscribe_int(&timer_bit_no)) 
        return 1;

    if (kbd_subscribe_int(&kbd_bit_no))
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
                if (msg.m_notify.interrupts & BIT(kbd_bit_no)) { /* subscribed interrupt */
                    kbc_ih();
                    if (kbc_ih_return == 1) {
                        fail = true;
                        break;
                    } else if (kbc_ih_return == 2) {
                        continue;
                    }
                
                    if (kbd_update_scancode(scancode, &size, bytes) != OK) {
                        fail = true;
                        break;
                    }
                }
                if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
                    timer_int_handler();
                    
                    if (interrupt_counter % interrupts_per_frame == 0) {
                        if (speed >= 0 || interrupt_counter % (-speed * interrupts_per_frame) == 0) {
                            if (sprite->x != xf || sprite->y != yf) {
                                if ((xf > sprite->x && xf < sprite->x + sprite->xspeed)
                                    || (xf < sprite->x && xf > sprite->x + sprite->xspeed)
                                    || (yf > sprite->y && yf < sprite->y + sprite->yspeed)
                                    || (yf < sprite->y && yf > sprite->y + sprite->yspeed)) {
                                    sprite->x = xf;
                                    sprite->y = yf;
                                } else {
                                    animate_sprite(sprite);
                                }
                            } else {
                                sprite->xspeed = 0;
                                sprite->yspeed = 0;
                                speed = 0;
                            }
                        }

                        if (vg_clear() != OK)
                            return 1;
                        if (draw_sprite(sprite) != OK)
                            return 1;
                        if (flip_page())
                            return 1;
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
    
    if (timer_unsubscribe_int() != OK) 
        return 1;

    if (vg_exit() != OK)
        return 1;

    return fail;
}

int(video_test_controller)() {
    vg_vbe_contr_info_t info_p;
    if (vbe_get_contr_info(&info_p) != OK)
        return 1;

    if (vg_display_vbe_contr_info(&info_p) != OK)
        return 1;

    return 0;
}
