#include <lcom/lcf.h>
#include "date.h"
#include "rtc.h"
#include "font.h"
#include "video_gr.h"

int date_bcd_to_binary(date_t *date) {
    if (date == NULL)
        return 1;

    date->year = BCD_TO_BINARY(date->year);
    date->month = BCD_TO_BINARY(date->month);
    date->day = BCD_TO_BINARY(date->day);
    date->hour = BCD_TO_BINARY(date->hour);
    date->minute = BCD_TO_BINARY(date->minute);
    date->second = BCD_TO_BINARY(date->second);
    return 0;
}

int date_draw(date_t date, uint16_t x, uint16_t y) {
    char date_str[DATE_STRING_SIZE];

    sprintf(date_str, "%04d/%02d/%02d %02d:%02d:%02d", date.year, date.month, date.day, date.hour, date.minute, date.second);

    if (vb_draw_rectangle(vg_get_back_buffer(), x-DATE_DISPLAY_BORDER, y-DATE_DISPLAY_BORDER, (DATE_STRING_SIZE-1)*CHAR_SPACE + 2*DATE_DISPLAY_BORDER, FONT_CHAR_HEIGHT + 2*DATE_DISPLAY_BORDER, DATE_BACK_COLOR) != OK)
        return 1;

    if (font_draw_string(vg_get_back_buffer(), date_str, x, y) != OK)
        return 1;

    return 0;
}

int date_draw_current() {
    date_t date;
    if (rtc_get_current_date(&date) != OK)
        return 1;
    if (date_draw(date, DATE_CURRENT_X, DATE_CURRENT_Y) != OK)
        return 1;
    return 0;
}

int date_draw_greeting(uint16_t x, uint16_t y) {
    date_t date;
    if (rtc_get_current_date(&date) != OK)
        return 1;

    char greeting[DATE_GREETING_MAX_SIZE];
    if (4 < date.hour && date.hour < 12) {
        strcpy(greeting, "GOOD MORNING");
    } else if (12 <= date.hour && date.hour < 19) {
        strcpy(greeting, "GOOD AFTERNOON");
    } else {
        strcpy(greeting, "GOOD NIGHT");
    }

    if (font_draw_string(vg_get_back_buffer(), greeting, x, y) != OK)
        return 1;

    return 0;
}

bool date_operator_less_than(date_t date1, date_t date2) {
    if (date1.year != date2.year)
        return date1.year < date2.year;
    if (date1.month != date2.month)
        return date1.month < date2.month;
    if (date1.day != date2.day)
        return date1.day < date2.day;
    if (date1.hour != date2.hour)
        return date1.hour < date2.hour;
    if (date1.minute != date2.minute)
        return date1.minute < date2.minute;
    return date1.second < date2.second;
}

static bool date_year_is_leap(uint16_t year) {
    if (year <= 1582)
        return 1; // gregorian calendar (15-10-1582)

    if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
        return true;
    
    return false;
}

static uint8_t date_get_month_num_days(date_t date) {
    if (date.month == 2) {
        if (date_year_is_leap(date.year)) {
            return 29;
        } else { 
            return 28;
        }
    } else if (date.month <= 7){
        return 30 + date.month % 2;
    } else {
        return 31 - date.month % 2;
    }
}

int date_plus_alarm_time(rtc_alarm_time_t alarm, date_t *date) { // values in binary (not BCD)
    if (date == NULL)
        return 1;
    uint8_t month_num_days = date_get_month_num_days(*date);

    uint8_t m_left = (alarm.seconds + date->second) / 60;
    date->second = (alarm.seconds + date->second) % 60;

    uint8_t h_left = (m_left + alarm.minutes + date->minute) / 60;
    date->minute = (m_left + alarm.minutes + date->minute) % 60;

    uint8_t d_left = (h_left + alarm.hours + date->hour) / 24;
    date->hour = (h_left + alarm.hours + date->hour) % 24;

    uint8_t mth_left = (d_left + date->day - 1) / month_num_days;
    date->day = (d_left + date->day - 1) % month_num_days + 1;

    uint8_t y_left = (mth_left + date->month - 1) / 12;
    date->month = (mth_left + date->month - 1) % 12 + 1;

    date->year = y_left + date->year;

    return 0;
}
