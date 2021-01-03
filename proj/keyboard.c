#include <lcom/lcf.h>
#include "keyboard.h"
#include "kbc.h"
#include "i8042.h"
#include "scan_codes.h"
#include "dispatcher.h"

/** @defgroup keyboard keyboard
 * @{
 *
 */

static int hook_id_kbd = 1; /**< @brief Keyboard interrupts hook id */
static uint8_t scancode_bytes[2]; /**< @brief Buffer for bytes read from KBC that represent a scancode */
static size_t scancode_bytes_counter = 0; /**< @brief Number meaningful of bytes in scancode_bytes buffer */
static kbd_event_t kbd_state = { .key = NO_KEY }; /**< @brief Current keyboard state */

int kbd_subscribe_int(uint8_t *bit_no) {
    *bit_no = hook_id_kbd;
    return sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd);
}

int kbd_unsubscribe_int() {
    return sys_irqrmpolicy(&hook_id_kbd);
}

/**
 * @brief Keyboard interrupt handler.
 * 
 */
void (kbc_ih)() {
    if (kbd_is_scancode_ready()) {
        printf("keyboard interrupt handler failed\n");
        return;
    }

    uint8_t data; 
    if (kbc_read_data(&data)) {
        printf("keyboard interrupt handler failed\n");
        return;
    }

    scancode_bytes[scancode_bytes_counter++] = data;

    if (kbd_is_scancode_ready()) {
        if (dispatcher_queue_event(KEYBOARD_EVENT) != OK) {
            printf("Failed to queue keyboard event\n");
        } 
    }
}

bool kbd_is_make_code(uint8_t scancode) {
    return (scancode & BREAK_CODE_BIT) == 0;
}

bool kbd_is_scancode_ready() {
   return scancode_bytes_counter == 2
        || (scancode_bytes_counter == 1 && scancode_bytes[0] != FIRST_BYTE_TWO_BYTE_SCANCODE);
}

int kbd_handle_scancode(kbd_event_t *kbd_event) {
    if (!kbd_is_scancode_ready())
        return 1;
    
    uint16_t code;
    if (scancode_bytes_counter == 1)
        code = scancode_bytes[0] | 0x0000;
    else if (scancode_bytes_counter == 2)
    	code = (scancode_bytes[0] << 8) | scancode_bytes[1];
    else {
        printf("error in %s", __func__);
        return 1;
    }
    
    if (!kbd_is_make_code(code)) {
        kbd_state.key = NO_KEY;
        if (code == BREAK_CODE(MAKE_CTRL)) {
            kbd_state.is_ctrl_pressed = false;
        }
    } else { // make code
        switch (code) {
	    case MAKE_CTRL:
	        kbd_state.key = CTRL;
            kbd_state.is_ctrl_pressed = true;
            break;
	                                     
	    case MAKE_ENTER:       kbd_state.key = ENTER;       break;
	    case MAKE_BACK_SPACE:  kbd_state.key = BACK_SPACE;  break;
	    case MAKE_ESC:         kbd_state.key = ESC;         break;
	    case MAKE_DEL:         kbd_state.key = DEL;         break;
	    case MAKE_ARROW_UP:    kbd_state.key = ARROW_UP;    break;
	    case MAKE_ARROW_DOWN:  kbd_state.key = ARROW_DOWN;  break;
	    case MAKE_ARROW_LEFT:  kbd_state.key = ARROW_LEFT;  break;
	    case MAKE_ARROW_RIGHT: kbd_state.key = ARROW_RIGHT; break;

        case MAKE_SPACE: kbd_state.key = CHAR; kbd_state.char_key = ' '; break;
	    case MAKE_A:     kbd_state.key = CHAR; kbd_state.char_key = 'A'; break;
	    case MAKE_B:     kbd_state.key = CHAR; kbd_state.char_key = 'B'; break;
	    case MAKE_C:     kbd_state.key = CHAR; kbd_state.char_key = 'C'; break;
	    case MAKE_D:     kbd_state.key = CHAR; kbd_state.char_key = 'D'; break;
	    case MAKE_E:     kbd_state.key = CHAR; kbd_state.char_key = 'E'; break;
	    case MAKE_F:     kbd_state.key = CHAR; kbd_state.char_key = 'F'; break;
	    case MAKE_G:     kbd_state.key = CHAR; kbd_state.char_key = 'G'; break;
	    case MAKE_H:     kbd_state.key = CHAR; kbd_state.char_key = 'H'; break;
	    case MAKE_I:     kbd_state.key = CHAR; kbd_state.char_key = 'I'; break;
	    case MAKE_J:     kbd_state.key = CHAR; kbd_state.char_key = 'J'; break;
	    case MAKE_K:     kbd_state.key = CHAR; kbd_state.char_key = 'K'; break;
	    case MAKE_L:     kbd_state.key = CHAR; kbd_state.char_key = 'L'; break;
	    case MAKE_M:     kbd_state.key = CHAR; kbd_state.char_key = 'M'; break;
	    case MAKE_N:     kbd_state.key = CHAR; kbd_state.char_key = 'N'; break;
	    case MAKE_O:     kbd_state.key = CHAR; kbd_state.char_key = 'O'; break;
	    case MAKE_P:     kbd_state.key = CHAR; kbd_state.char_key = 'P'; break;
	    case MAKE_Q:     kbd_state.key = CHAR; kbd_state.char_key = 'Q'; break;
	    case MAKE_R:     kbd_state.key = CHAR; kbd_state.char_key = 'R'; break;
	    case MAKE_S:     kbd_state.key = CHAR; kbd_state.char_key = 'S'; break;
	    case MAKE_T:     kbd_state.key = CHAR; kbd_state.char_key = 'T'; break;
	    case MAKE_U:     kbd_state.key = CHAR; kbd_state.char_key = 'U'; break;
	    case MAKE_V:     kbd_state.key = CHAR; kbd_state.char_key = 'V'; break;
    	case MAKE_W:     kbd_state.key = CHAR; kbd_state.char_key = 'W'; break;
	    case MAKE_X:     kbd_state.key = CHAR; kbd_state.char_key = 'X'; break;
	    case MAKE_Y:     kbd_state.key = CHAR; kbd_state.char_key = 'Y'; break;
    	case MAKE_Z:     kbd_state.key = CHAR; kbd_state.char_key = 'Z'; break;
	
	    case MAKE_0: kbd_state.key = CHAR; kbd_state.char_key = '0'; break;
	    case MAKE_1: kbd_state.key = CHAR; kbd_state.char_key = '1'; break;
	    case MAKE_2: kbd_state.key = CHAR; kbd_state.char_key = '2'; break;
	    case MAKE_3: kbd_state.key = CHAR; kbd_state.char_key = '3'; break;
	    case MAKE_4: kbd_state.key = CHAR; kbd_state.char_key = '4'; break;
	    case MAKE_5: kbd_state.key = CHAR; kbd_state.char_key = '5'; break;
	    case MAKE_6: kbd_state.key = CHAR; kbd_state.char_key = '6'; break;
	    case MAKE_7: kbd_state.key = CHAR; kbd_state.char_key = '7'; break;
	    case MAKE_8: kbd_state.key = CHAR; kbd_state.char_key = '8'; break;
	    case MAKE_9: kbd_state.key = CHAR; kbd_state.char_key = '9'; break;
		default:     kbd_state.key = NO_KEY; // for keys not mapped
        }
    }

    scancode_bytes_counter = 0;
    *kbd_event = kbd_state;
    return 0;
}

int kbd_enable_interrupts() {
    uint8_t cmd;
    if (kbc_read_command_byte(&cmd)) 
        return 1;
    cmd |= CMD_BYTE_ENABLE_KBD_INT;
    if (kbc_write_command_byte(cmd)) 
        return 1;
    return 0;
}

/**@}*/
