#ifndef __RTC_H
#define __RTC_H

#include <lcom/lcf.h>

#define RTC_IRQ 8
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

#define RTC_PF BIT(6)
#define RTC_AF BIT(5)
#define RTC_UF BIT(4)

#define RTC_PIE BIT(6)
#define RTC_AIE BIT(5)
#define RTC_UIE BIT(4)
#define RTC_MASK_DISABLE(bit) (~((uint8_t)(bit)))

#define RTC_REGISTER_SECONDS 0x0
#define RTC_REGISTER_SECONDS_ALARM 0x1
#define RTC_REGISTER_MINUTES 0x2
#define RTC_REGISTER_MINUTES_ALARM 0x3
#define RTC_REGISTER_HOURS 0x4
#define RTC_REGISTER_HOURS_ALARM 0x5
#define RTC_REGISTER_DAY_OF_THE_WEAK 0x6
#define RTC_REGISTER_DAY_OF_THE_MONTH 0x7
#define RTC_REGISTER_MONTH 0x8
#define RTC_REGISTER_YEAR 0x9
#define RTC_REGISTER_A 0xA
#define RTC_REGISTER_B 0xB
#define RTC_REGISTER_C 0xC
#define RTC_REGISTER_D 0xD


typedef enum rtc_interrupt_t {
    PERIODIC_INTERRUPT,
    ALARM_INTERRUPT,
    UPDATE_INTERRUPT    
} rtc_interrupt_t;

typedef struct date_t {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} date_t;

int rtc_read_conf(); // test function 2013/2014

int rtc_read_date(); // test function 2013/2014

int rtc_subscribe_int(uint8_t *bit_no);

int rtc_unsubscribe_int();

int rtc_enable_int(rtc_interrupt_t rtc_interrupt);

int rtc_disable_int(rtc_interrupt_t rtc_interrupt);

int rtc_read_register(uint8_t adress, uint8_t *value);

int rtc_write_register(uint8_t address, uint8_t value);

void rtc_ih();

#endif /* __RTC_H */
