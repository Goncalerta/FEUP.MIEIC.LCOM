#ifndef __RTC_H
#define __RTC_H

#include <lcom/lcf.h>
#include "date.h"

/** @defgroup rtc rtc
 * @{
 *
 * @brief Module to interact with the RTC.
 */

#define RTC_IRQ 8 /**< @brief RTC IRQ line */

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
    rtc_alarm_time_t alarm_time; /*!< Time to set the alarm to (binary format). */
    uint8_t periodic_RS3210; /*!< RS3 RS2 RS1 RS0 in 4 LS bits used to setup periodic interrupts. */
} rtc_interrupt_config_t;

/**
 * @brief Prints the content of RTC registers A, B, C and D.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_read_conf(); // test function 2013/2014 // TODO delete? or let it be?

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
 * @return 
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
 * @param config interrupt configuration (not used if \link rtc_interrupt \endlink == \link UPDATE_INTERRUPT \endlink) // TODO how to make "rtc_interrupt" link to the parameter?
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
 * @return Return the seed 
 */
unsigned int rtc_get_seed();

/**@}*/

#endif /* __RTC_H */
