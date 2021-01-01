#ifndef __RTC_H
#define __RTC_H

#include <lcom/lcf.h>
#include "date.h"

/** @file 
 * @brief File dedicated to the interaction with the RTC.
 */

/** @defgroup rtc rtc
 * @{
 *
 * @brief Module to interact with the RTC.
 */

#define RTC_IRQ 8 /**< @brief RTC IRQ line. */

#define RTC_ADDR_REG 0x70 /**< @brief RTC address register. */
#define RTC_DATA_REG 0x71  /**< @brief RTC data register. */

#define RTC_UIP BIT(7) /**< @brief Update in progress bit. */
#define RTC_DELAY_U 244 /**< @brief Time interval (micros) during which UIP is set. */

#define RTC_PF BIT(6) /**< @brief Periodic interrupt pending bit. */
#define RTC_AF BIT(5) /**< @brief Alarm interrupt pending bit. */
#define RTC_UF BIT(4) /**< @brief Update interrupt pending bit. */

#define RTC_PIE BIT(6) /**< @brief Enable periodic interrupts bit. */
#define RTC_AIE BIT(5) /**< @brief Enable alarm interrupts bit. */
#define RTC_UIE BIT(4) /**< @brief Enable update interrupts bit. */
#define RTC_MASK_DISABLE(byte) (~((uint8_t)(byte))) /**< @brief Mask used to disable the correspondent interrupt to the bit set in byte. */

#define RTC_REGISTER_SECONDS 0x0 /**< @brief RTC seconds register. */
#define RTC_REGISTER_SECONDS_ALARM 0x1 /**< @brief RTC alarm seconds register. */
#define RTC_REGISTER_MINUTES 0x2 /**< @brief RTC minutes register. */
#define RTC_REGISTER_MINUTES_ALARM 0x3 /**< @brief RTC alarm minutes register. */
#define RTC_REGISTER_HOURS 0x4 /**< @brief RTC hours register. */
#define RTC_REGISTER_HOURS_ALARM 0x5 /**< @brief RTC alarm hours register. */
#define RTC_REGISTER_DAY_OF_THE_WEAK 0x6 /**< @brief RTC day of the weak register. */
#define RTC_REGISTER_DAY_OF_THE_MONTH 0x7 /**< @brief RTC day of the month register. */
#define RTC_REGISTER_MONTH 0x8 /**< @brief RTC month register. */
#define RTC_REGISTER_YEAR 0x9 /**< @brief RTC year register. */
#define RTC_REGISTER_A 0xA /**< @brief RTC A register. */
#define RTC_REGISTER_B 0xB /**< @brief RTC B register. */
#define RTC_REGISTER_C 0xC /**< @brief RTC C register. */
#define RTC_REGISTER_D 0xD /**< @brief RTC D register. */

/**
 * @brief Enumerated type for specifying a RTC interrupt type.
 * 
 */
typedef enum rtc_interrupt_t {
    PERIODIC_INTERRUPT, /*!< Periodic interrupt. */
    ALARM_INTERRUPT, /*!< Alarm interrupt. */
    UPDATE_INTERRUPT /*!< Update interrupt. */
} rtc_interrupt_t;

/**
 * @brief RTC configuration to enable interrupts.
 * 
 */
typedef union rtc_interrupt_config_t {
    rtc_alarm_time_t alarm_time; /*!< @brief Time to set the alarm to (binary format). */
    uint8_t periodic_RS3210; /*!< @brief RS3 RS2 RS1 RS0 in 4 LS bits used to setup periodic interrupts. */
} rtc_interrupt_config_t;

/**
 * @brief Prints the content of RTC registers A, B, C and D.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_read_conf();

/**
 * @brief Subscribes RTC interrupts.
 * 
 * @param bit_no address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes Keyboard interrupts.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_unsubscribe_int();

/**
 * @brief Reads the current date from the RTC and stores it inside the rtc module.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_read_date();

/**
 * @brief Gets the current date from inside the rtc module.
 * 
 * @param date address of memory to be initialized with the current date
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_get_current_date(date_t *date);

/**
 * @brief Enables RTC update interrupts.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_enable_update_int();

/**
 * @brief Sets an alarm with remaining time to alarm from now.
 * 
 * @param remaining_time_to_alarm remaining time to receive an alarm interrut
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_set_alarm_in(rtc_alarm_time_t remaining_time_to_alarm);

/**
 * @brief Enables a type of interrupt with the given configuration.
 * 
 * @param rtc_interrupt interrupt type to enable
 * @param config interrupt configuration (not used if rtc_interrupt == UPDATE_INTERRUPT)
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_enable_int(rtc_interrupt_t rtc_interrupt, rtc_interrupt_config_t config);

/**
 * @brief Disables a type of interrupt.
 * 
 * @param rtc_interrupt interrupt type to disable
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_disable_int(rtc_interrupt_t rtc_interrupt);

/**
 * @brief Reads data from a RTC register.
 * 
 * @param reg RTC register
 * @param value address of memory to be initialized with the data that was read
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_read_register(uint8_t reg, uint8_t *value);

/**
 * @brief Writes data to a RTC register.
 * 
 * @param reg RTC register
 * @param value data to write
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_write_register(uint8_t reg, uint8_t value);

/**
 * @brief RTC interrupt handler.
 * 
 */
void rtc_ih();

/**
 * @brief Flushes RTC older interrupt notifications by reading the register C.
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int rtc_flush();

/**
 * @brief Generates a seed calculated with the current date.
 * 
 * This is used to initialize the random number generator.
 * 
 * @return Return the seed 
 */
unsigned int rtc_get_seed();

/**@}*/

#endif /* __RTC_H */
