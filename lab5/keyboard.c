#include <lcom/lcf.h>
#include "keyboard.h"
#include "kbc.h"
#include "i8042.h"

int hook_id_kbd = KEYBOARD_IRQ;

int (kbd_subscribe_int)(uint8_t *bit_no) {
    *bit_no = hook_id_kbd;
    return sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd);
}

int (kbd_unsubscribe_int)() {
    return sys_irqrmpolicy(&hook_id_kbd);
}

void (kbc_ih)() {
    kbc_ih_return = kbc_read_data(&scancode);
}

bool is_make_code(uint8_t scancode) {
    return (scancode & BREAKCODE_BIT) == 0;
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

int (kbd_update_scancode)(uint8_t scancode, uint8_t *size, uint8_t *bytes) {
    if (*size == 2) {
        bytes[1] = scancode;
        *size = 1;
    } else {
        bytes[0] = scancode;
        if (scancode == FIRST_BYTE_TWO_BYTE_SCANCODE) {
            *size = 2;
        }
    }

    return 0;
}

int (kbd_display_scancode)(uint8_t scancode, uint8_t *size, uint8_t *bytes) {
    if (*size == 2) {
        bytes[1] = scancode;
        
        if(kbd_print_scancode(is_make_code(scancode), *size, bytes))
            return 1;
        
        *size = 1;
    } else {
        bytes[0] = scancode;
        if (scancode == FIRST_BYTE_TWO_BYTE_SCANCODE) {
            *size = 2;
        } else {
            if(kbd_print_scancode(is_make_code(scancode), *size, bytes)) 
                return 1;
        }
    }

    return 0;
}
