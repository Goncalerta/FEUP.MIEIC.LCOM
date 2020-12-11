#include <lcom/lcf.h>

#include "rtc.h"

static int hook_id_rtc = 3;
//static date_t current_date;

// this will probably be deleted/modified
static void rtc_print_byte_binary_format(uint8_t val) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (val>>i) & 0x1);
    }
    printf("\n");
}

int rtc_read_conf() {
    uint8_t reg_a, reg_b, reg_c, reg_d;
    if (rtc_read_register(RTC_REGISTER_A, &reg_a) != 0)
        return 1;
    if (rtc_read_register(RTC_REGISTER_B, &reg_b) != 0)
        return 1;
    if (rtc_read_register(RTC_REGISTER_C, &reg_c) != 0)
        return 1;
    if (rtc_read_register(RTC_REGISTER_D, &reg_d) != 0)
        return 1;

    printf("REGISTER A: ");
    rtc_print_byte_binary_format(reg_a);
    printf("REGISTER B: ");
    rtc_print_byte_binary_format(reg_b);
    printf("REGISTER C: ");
    rtc_print_byte_binary_format(reg_c);
    printf("REGISTER D: ");
    rtc_print_byte_binary_format(reg_d);

    return 0;
}

int rtc_read_date() {
    // TODO
    return 0;
}

int rtc_subscribe_int(uint8_t *bit_no) {
    *bit_no = hook_id_rtc;
    return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &hook_id_rtc); // TODO does it need to be IRQ_EXCLUSIVE?
}

int rtc_unsubscribe_int() {
    return sys_irqrmpolicy(&hook_id_rtc);
}

// TODO maybe it will be a function per interrupt type
int rtc_enable_int(rtc_interrupt_t rtc_interrupt) {
    uint8_t reg_b = 0;
    if (rtc_read_register(RTC_REGISTER_B, &reg_b) != 0) {
        return 1;
    }

    // it doesn't set the config values (the rtc_enable_int() doesn't have them yet)
    switch (rtc_interrupt) {
    case ALARM_INTERRUPT:
        reg_b |= RTC_AIE;
        break;

    case UPDATE_INTERRUPT:
        reg_b |= RTC_UIE;
        break;
      
    case PERIODIC_INTERRUPT:
        reg_b |= RTC_PIE;
        break;
    }

    if (rtc_write_register(RTC_REGISTER_B, reg_b) != 0) {
        return 1;
    }

    return 0;
}

int rtc_disable_int(rtc_interrupt_t rtc_interrupt) {
    uint8_t reg_b = 0;
    if (rtc_read_register(RTC_REGISTER_B, &reg_b) != 0) {
        return 1;
    }

    switch (rtc_interrupt) {
    case ALARM_INTERRUPT:
        reg_b &= RTC_MASK_DISABLE(RTC_AIE);
        break;

    case UPDATE_INTERRUPT:
        reg_b &= RTC_MASK_DISABLE(RTC_UIE);
        break;
      
    case PERIODIC_INTERRUPT:
        reg_b &= RTC_MASK_DISABLE(RTC_PIE);
        break;
    }

    if (rtc_write_register(RTC_REGISTER_B, reg_b) != 0) {
        return 1;
    }

    return 0;
}

int rtc_read_register(uint8_t address, uint8_t *value) {
    if (sys_outb(RTC_ADDR_REG, address) != 0) {
        return 1;
    }
    // TODO some tickdelay?
    // TODO slide 10, é preciso "Disable interrupts before starting to read" ? 

    if (util_sys_inb(RTC_DATA_REG, value) != 0) {
        return 1;
    }
    
    return 0;
}

int rtc_write_register(uint8_t address, uint8_t value) {
    if (sys_outb(RTC_ADDR_REG, address) != 0) {
        return 1;
    }
    // TODO some tickdelay?
    if (sys_outb(RTC_DATA_REG, value) != 0) {
        return 1;
    }
    
    return 0;
}

void rtc_ih() {
    uint8_t register_c = 0;
    if (rtc_read_register(RTC_REGISTER_C, &register_c) != 0) {
        printf("Error while reading register C\n");
        return; // TODO have some global var to signal an error?
    }

    // TODO
    if (register_c & RTC_PF) {
        //printf("RTC periodic interrupt detected\n");
    }
    if (register_c & RTC_AF) {
        //printf("RTC alarm interrupt detected\n");
    }
    if (register_c & RTC_UF) {
        //printf("RTC update interrupt detected\n");
    }
}
