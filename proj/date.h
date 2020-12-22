#ifndef __DATE_H
#define __DATE_H

#include <lcom/lcf.h>

#define DATE_CURRENT_X 5 // TODO chose the right place to draw
#define DATE_CURRENT_Y 5
#define DATE_STRING_SIZE 20
#define DATE_DISPLAY_BORDER 4
#define DATE_BACK_COLOR 0xffffff
#define DATE_GREETING_MAX_SIZE 15


#define BCD_TO_BINARY(byte) (((byte) & 0x0f) + (((byte) & 0xf0) >> 4)*10)
#define BINARY_TO_BCD(byte) (((byte)/10) << 4 | (byte) % 10)

typedef struct date_t {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} date_t;


int date_bcd_to_binary(date_t *date);

int date_draw(date_t date, uint16_t x, uint16_t y);

int date_draw_current();

int date_draw_greeting(uint16_t x, uint16_t y);


#endif /* __DATE_H */
