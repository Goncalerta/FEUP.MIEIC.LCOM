#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int interrupt_counter = 0;
int hook_id_timer = TIMER0_IRQ;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
    if (timer > 2 || freq < 19 || freq > TIMER_FREQ)
        return 1;

    uint8_t config, c_word;
    if(timer_get_conf(timer, &config))
        return 1;
    
    switch (timer) {
    case 0:
        c_word = TIMER_SEL0;
        break;
    case 1:
        c_word = TIMER_SEL1;
        break;
    case 2:
        c_word = TIMER_SEL2;
        break;
    }

    c_word |= TIMER_LSB_MSB | (config & (MASK_BASE | MASK_MODE));

    // freq = clock/div <=> div = clock/freq
    uint16_t div = TIMER_FREQ / freq;
    uint8_t div_LSB, div_MSB;

    if (util_get_LSB(div, &div_LSB) != OK)
        return 1;
    if (util_get_MSB(div, &div_MSB) != OK)
        return 1;

    if (sys_outb(TIMER_CTRL, c_word) != OK)
        return 1;
    if (sys_outb(TIMER(timer), div_LSB) != OK)
        return 1;
    if (sys_outb(TIMER(timer), div_MSB) != OK)
        return 1;
    
    return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    *bit_no = hook_id_timer;
    return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer);
}

int (timer_unsubscribe_int)() {
    return sys_irqrmpolicy(&hook_id_timer);
}

void (timer_int_handler)() {
    interrupt_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
    if (timer > 2) 
        return 1;

    uint32_t read_back_command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

    if (sys_outb(TIMER_CTRL, read_back_command) != OK) 
        return 1;
    if (util_sys_inb(TIMER(timer), st) != OK)
        return 1;

    return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                         enum timer_status_field field) {
    if (timer > 2) 
        return 1;
  
    union timer_status_field_val conf;
    
    switch (field) {
    case tsf_all: 
        conf.byte = st; 
        break;
    case tsf_initial: 
        switch ((st & MASK_INITIAL) >> 4) {
        case 1:
            conf.in_mode = LSB_only;
            break;
        case 2:
            conf.in_mode = MSB_only;
            break;
        case 3:
            conf.in_mode = MSB_after_LSB;
            break;
        default:
            conf.in_mode = INVAL_val;
            break;
        }
        break;
    case tsf_mode: 
        conf.count_mode = (st & MASK_MODE) >> 1; 
        if (conf.count_mode >= 6) 
            conf.count_mode -= 4;
        break;
    case tsf_base: 
        conf.bcd = st & MASK_BASE; 
        break;
    }

    return timer_print_config(timer, field, conf);
}
