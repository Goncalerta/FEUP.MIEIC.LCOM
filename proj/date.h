#ifndef __DATE_H
#define __DATE_H

#include <lcom/lcf.h>

/** @file 
 * @brief File dedicated to dates.
 */

/** @defgroup date date
 * @{
 *
 *  @brief Module dedicated to dates.
 */

#define BCD_TO_BINARY(byte) (((byte) & 0x0f) + (((byte) & 0xf0) >> 4)*10) /**< @brief BYTE conversion from BCD to binary format */
#define BINARY_TO_BCD(byte) (((byte)/10) << 4 | (byte) % 10) /**< @brief BYTE conversion from binary to BCD format */

/**
 * @brief Date info.
 * 
 */
typedef struct date {
    uint16_t year; /*!< @brief Year. */
    uint8_t month; /*!< @brief Month. */
    uint8_t day; /*!< @brief Day. */
    uint8_t hour; /*!< @brief Hour. */
    uint8_t minute; /*!< @brief Minute. */
    uint8_t second; /*!< @brief Second. */
} date_t;

/**
 * @brief Time aggregate type to use to set the RTC alarm.
 * 
 */
typedef struct rtc_alarm_time {
    uint8_t hours; /*!< @brief Hours. */
    uint8_t minutes; /*!< @brief Minutes. */
    uint8_t seconds; /*!< @brief Seconds. */
} rtc_alarm_time_t;


/**
 * @brief Converts a date_t from BCD to binary format.
 * 
 * @param date address of memory of the date to convert
 * @return Return 0 upon success and non-zero otherwise
 */
int date_bcd_to_binary(date_t *date);

/**
 * @brief Draws a date to the given screen coordiantes.
 * 
 * @param date date to draw
 * @param x x coordinate
 * @param y y coordinate
 * @return Return 0 upon success and non-zero otherwise
 */
int date_draw(date_t date, uint16_t x, uint16_t y);

/**
 * @brief Draws the current date to the top left corner of the screen.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int date_draw_current();

/**
 * @brief Draws a greeting according to the current date to the given screen coordinates.
 * 
 * @param x x coordinate
 * @param y y coordinate
 * @return Return 0 upon success and non-zero otherwise
 */
int date_draw_greeting(uint16_t x, uint16_t y);

/**
 * @brief Compares 2 date_t.
 * 
 * @param date1 date to compare
 * @param date2 date to compare
 * @return Return true if date1 is before date2 and false otherwise
 */
bool date_operator_less_than(date_t date1, date_t date2);

/**
 * @brief Adds a date_t to a rtc_alarm_time_t.
 * 
 * @param alarm alarm time to add
 * @param date address of memory both to the date to add and result of the addition
 * @return Return 0 upon success and non-zero otherwise
 */
int date_plus_alarm_time(rtc_alarm_time_t alarm, date_t *date);

/**@}*/

#endif /* __DATE_H */
