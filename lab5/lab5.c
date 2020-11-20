// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "defines_graphic.h"
#include "i8042.h"
#include "keyboard.h"

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
  if (vg_init(mode) == NULL) {
    return 1;
  }
  tickdelay(micros_to_ticks(delay * SECONDS_TO_MICROS));
  if (vg_exit()) {
    return 1;
  }
  return 0;
}

/*The test of these functions is based essentially on the contents of the frame-buffer upon calling vg_exit(). Therefore, you should invoke VBE function 0x02, Set VBE Mode, with bit 15 of the BX register cleared, thus ensuring that the display memory is cleared after switching to the desired graphics mode.*/
//TODO ^^ a vg_exit() não faz já isto ao dar memset com 0's?
int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  
  int ipc_status;
  message msg;
  uint8_t bit_no = KEYBOARD_IRQ;
  int r;
  uint8_t bytes[] = {0, 0};
  bool reading_2nd_byte = false;
  int fail = 0;

  if (vg_init(mode) == NULL) {
    return 1;
  }
  if (vg_draw_rectangle(x, y, width, height, color)) {
    return 1;
  }

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
            if (ih_return == 1) {
              fail = 1;
              break;
            } else if (ih_return == 2) {
              continue;
            }
            
            if (reading_2nd_byte) {
              bytes[1] = scancode;
              reading_2nd_byte = false;
            } else {
              bytes[0] = scancode;
              if (scancode == FIRST_BYTE_TWO_BYTE_SCANCODE) {
                reading_2nd_byte = true;
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

  if (vg_exit()) {
    return 1;
  }
  
  return keyboard_unsubscribe_int() || fail;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  /* To be completed */
  printf("%s(0x%03x, %u, 0x%08x, %d): under construction\n", __func__,
         mode, no_rectangles, first, step);

  return 1;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  /* To be completed */
  printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
