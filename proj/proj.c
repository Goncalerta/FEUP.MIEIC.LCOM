// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you
#include "kbc.h"
#include "keyboard.h"
#include "mouse.h"
#include "video_gr.h"
#include "canvas.h"
#include "cursor.h"
#include "font.h"
#include "dispatcher.h"
#include "textbox.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  //lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  //lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (proj_main_loop)(int argc, char *argv[]) {
    uint16_t mode = 0x118;
    enum xpm_image_type image_type = XPM_8_8_8;
    uint8_t timer_irq_set, kbd_irq_set, mouse_irq_set;

    if (vg_init(mode) == NULL) 
        return 1;

    if (timer_subscribe_int(&timer_irq_set) != OK) 
        return 1;

    if (kbd_subscribe_int(&kbd_irq_set) != OK) 
        return 1;

    if (mouse_enable_dr() != OK)
        return 1;

    if (mouse_subscribe_int(&mouse_irq_set) != OK) 
        return 1;

    // INIT game assets
    // TODO probably move those to a more appropriate place later in the project
    font_load(image_type);
    cursor_init(image_type);
    event_new_game();
    // ^^

    int ipc_status, r;
    message msg;
    while ( !should_end() ) {
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
            printf("driver_receive failed with: %d\n", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & BIT(mouse_irq_set)) {
                    mouse_ih();
                }
                if (msg.m_notify.interrupts & BIT(kbd_irq_set)) {
                    kbc_ih();
                    //using to debug
                    if (1) { 
                        text_box t_box;
                        char * word = "AI QUE GIRO";
                        t_box.word = word;
                        t_box.cursor_pos = 11;
                        t_box.select_pos = 7;
                        t_box.start_display = 3;
                        t_box.x_pos = TEXT_BOX_GUESS_X;
                        t_box.y_pos = TEXT_BOX_GUESS_Y;
                        t_box.height = TEXT_BOX_GUESS_HEIGHT;
                        t_box.width = TEXT_BOX_GUESS_WIDTH;
                        t_box.display_size = TEXT_BOX_GUESS_DISPLAY_SIZE;
                        t_box.beg_end_space = TEXT_BOX_GUESS_BEG_END_SPACE;
                        t_box.top_bot_space = TEXT_BOX_GUESS_TOP_BOT_SPACE;
                        if (text_box_draw(vg_get_back_buffer(), t_box, true) != 0) {
                            printf("text_box_draw failed\n");
                            return 1;
                        }
                        vg_flip_page();
                        tickdelay(micros_to_ticks(SECONDS_TO_MICROS*2));
                    }
                    //^^
                }
                if (msg.m_notify.interrupts & BIT(timer_irq_set)) {
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

    // EXIT game assets
    // TODO probably move those to a more appropriate place later in the project
    if (canvas_exit() != OK)
        return 1;
    // ^^
    
    if (kbd_unsubscribe_int() != OK)
        return 1;

    if (mouse_unsubscribe_int() != OK) 
        return 1;
    
    if (mouse_disable_dr() != OK) 
        return 1;
    
    if (kbc_flush() != OK)
        return 1;

    if (timer_unsubscribe_int() != OK)
        return 1;

    if (vg_exit() != OK)
        return 1;

    return 0;
}
