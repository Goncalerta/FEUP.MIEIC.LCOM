#include <lcom/lcf.h>
#include "keyboard.h"
#include "kbc.h"
#include "i8042.h"

static int hook_id_kbd = KEYBOARD_IRQ;
static uint8_t scancode_bytes[2];
static size_t scancode_bytes_counter = 0;
bool should_retrieve = false;

int kbd_subscribe_int(uint8_t *bit_no) {
    *bit_no = hook_id_kbd;
    return sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd);
}

int kbd_unsubscribe_int() {
    return sys_irqrmpolicy(&hook_id_kbd);
}

void (kbc_ih)() {
    if (should_retrieve) {
        kbd_ih_return = 1;
        return;
    }

    uint8_t data; 
    if (kbc_read_data(&data)) {
        kbd_ih_return = 1;
        return;
    }

    scancode_bytes[scancode_bytes_counter++] = data;
    if (scancode_bytes_counter == 2 || data != FIRST_BYTE_TWO_BYTE_SCANCODE) {
        should_retrieve = true;
    }

    kbd_ih_return = 0;
}

bool kbd_is_make_code(uint8_t scancode) {
    return (scancode & BREAKCODE_BIT) == 0;
}

bool kbd_scancode_ready() {
    return should_retrieve;
}

int kbd_handle_scancode() {
    if (!should_retrieve)
        return 1;
    
    // TODO
    if (scancode_bytes_counter == 1)
        printf("kbd_handle_scancode is not yet implemented. scancode: %d\n", scancode_bytes[0]);
    else if (scancode_bytes_counter == 2)
        printf("kbd_handle_scancode is not yet implemented. scancode: %d, %d\n", scancode_bytes[0], scancode_bytes[1]);
    
    scancode_bytes_counter = 0;
    should_retrieve = false;
    return 1;
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
