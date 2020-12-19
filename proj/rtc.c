#include <lcom/lcf.h>

#include "rtc.h"
#include "dispatcher.h"
#include "date.h"

static int hook_id_rtc = 3;
static date_t current_date;

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

int rtc_get_current_date(date_t *date) {
    if (date == NULL)
        return 1;
    if (memcpy(date, &current_date, sizeof(date_t)) == NULL)
        return 1;
    return 0;
}

int rtc_read_date() {
    // TODO isto ainda está incompleto?
    // é necessário dar disable no início e enable no final?
    // slide 10 e 20
    uint8_t reg_a;
    if (rtc_read_register(RTC_REGISTER_A, &reg_a) != 0)
        return 1;
    
    if (reg_a & RTC_UIP) { // slide 15
        tickdelay(micros_to_ticks(RTC_DELAY_U));
    }

    if (rtc_read_register(RTC_REGISTER_SECONDS, &current_date.second) != 0)
        return 1;
    if (rtc_read_register(RTC_REGISTER_MINUTES, &current_date.minute) != 0)
        return 1;
    if (rtc_read_register(RTC_REGISTER_HOURS, &current_date.hour) != 0)
        return 1;
    if (rtc_read_register(RTC_REGISTER_DAY_OF_THE_MONTH, &current_date.day) != 0)
        return 1;
    if (rtc_read_register(RTC_REGISTER_MONTH, &current_date.month) != 0)
        return 1;
    if (rtc_read_register(RTC_REGISTER_YEAR, &current_date.year) != 0)
        return 1;

    if (date_bcd_to_binary(&current_date) != 0)
        return 1;

    return 0;
}

int rtc_subscribe_int(uint8_t *bit_no) {
    *bit_no = hook_id_rtc;
    return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &hook_id_rtc); // TODO does it need to be IRQ_EXCLUSIVE?
}

int rtc_unsubscribe_int() {
    return sys_irqrmpolicy(&hook_id_rtc);
}

static int alarm_time_binary_to_bcd(rtc_alarm_time_t * time) {
    if (time == NULL)
        return 1;
    
    time->hours = BINARY_TO_BCD(time->hours);
    time->minutes = BINARY_TO_BCD(time->minutes);
    time->seconds = BINARY_TO_BCD(time->seconds);
    return 0;
}

static int alarm_time_plus_date(rtc_alarm_time_t *alarm, date_t date) { // values in binary (not BCD)
    if (alarm == NULL)
        return 1;

    uint8_t m_left = (alarm->seconds + date.second) / 60;
    alarm->seconds = (alarm->seconds + date.second) % 60;

    uint8_t h_left = (m_left + alarm->minutes + date.minute) / 60;
    alarm->minutes = (m_left + alarm->minutes + date.minute) % 60;

    alarm->hours = (h_left + alarm->hours + date.hour) % 24;
    return 0;
}

int rtc_enable_update_int() {
    rtc_interrupt_config_t config; // no config is used when enabling update interrupts
    if (rtc_enable_int(UPDATE_INTERRUPT, config) != 0)
        return 1;

    return 0;
}

int rtc_set_alarm_in(rtc_alarm_time_t remaining_time_to_alarm) {
    if (alarm_time_plus_date(&remaining_time_to_alarm, current_date) != 0) 
        return 1;
    
    rtc_interrupt_config_t config = {.alarm_time = remaining_time_to_alarm};
    if (rtc_enable_int(ALARM_INTERRUPT, config) != 0)
        return 1;
    
    return 0;
}

int rtc_enable_int(rtc_interrupt_t rtc_interrupt, rtc_interrupt_config_t config) {
    uint8_t reg_b = 0;
    if (rtc_read_register(RTC_REGISTER_B, &reg_b) != 0) {
        return 1;
    }

    switch (rtc_interrupt) {
    case ALARM_INTERRUPT:
        reg_b |= RTC_AIE;

        if (alarm_time_binary_to_bcd(&config.alarm_time) != 0)
            return 1;
        if (rtc_write_register(RTC_REGISTER_SECONDS_ALARM, config.alarm_time.seconds) != 0)
            return 1;
        if (rtc_write_register(RTC_REGISTER_MINUTES_ALARM, config.alarm_time.minutes) != 0)
            return 1;
        if (rtc_write_register(RTC_REGISTER_HOURS_ALARM, config.alarm_time.hours) != 0)
            return 1;

        break;

    case UPDATE_INTERRUPT:
        reg_b |= RTC_UIE;
        break;
      
    case PERIODIC_INTERRUPT:
        reg_b |= RTC_PIE;
        
        uint8_t reg_a;
        if (rtc_read_register(RTC_REGISTER_A, &reg_a) != 0)
            return 1;
        // TODO check if this is correct (can the code be preempted?)
        // if UIP is set, and meanwhile it becomes unset, does it make a problem? // does it become set? or the RTC corrects it to the correct value?
        reg_a = (reg_a & 0xf0) | (config.periodic_RS3210 & 0x0f);
        if (rtc_write_register(RTC_REGISTER_A, reg_a) != 0)
            return 1;

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
    // TODO is it better to reset the config values to "default"?
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

    if (util_sys_inb(RTC_DATA_REG, value) != 0) {
        return 1;
    }
    
    return 0;
}

int rtc_write_register(uint8_t address, uint8_t value) {
    if (sys_outb(RTC_ADDR_REG, address) != 0) {
        return 1;
    }
    
    if (sys_outb(RTC_DATA_REG, value) != 0) {
        return 1;
    }
    
    return 0;
}

void rtc_ih() {
    // TODO have some global var to signal an error in the IH?
    uint8_t register_c = 0;
    if (rtc_read_register(RTC_REGISTER_C, &register_c) != 0) {
        printf("Error while reading register C\n");
        return; 
    }

    if (register_c & RTC_UF) {
        if (rtc_read_date() != 0)
            return;
    }
    if (register_c & RTC_PF) {
        printf("RTC periodic interrupt detected.\n");
    }
    if (register_c & RTC_AF) {
        if (dispatch_rtc_alarm_int() != 0)
            return;
    }
}

int rtc_flush() {
    uint8_t register_c = 0;
    if (rtc_read_register(RTC_REGISTER_C, &register_c) != 0) {
        printf("Error while flushing register C\n");
        return 1;
    }
    return 0;
}
