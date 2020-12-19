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
    //TODO '/' ou '-' ?
    sprintf(date_str, "20%02d/%02d/%02d %02d:%02d:%02d", date.year, date.month, date.day, date.hour, date.minute, date.second);

    if (vb_draw_rectangle(vg_get_back_buffer(), x-DATE_DISPLAY_BORDER, y-DATE_DISPLAY_BORDER, (DATE_STRING_SIZE-1)*CHAR_SPACE + 2*DATE_DISPLAY_BORDER, FONT_CHAR_HEIGHT + 2*DATE_DISPLAY_BORDER, DATE_BACK_COLOR) != 0)
        return 1;

    if (font_draw_string(vg_get_back_buffer(), date_str, x, y, 0, 100) != 0)
        return 1;

    return 0;
}

int date_draw_current() {
    date_t date;
    if (rtc_get_current_date(&date) != 0)
        return 1;
    if (date_draw(date, DATE_CURRENT_X, DATE_CURRENT_Y) != 0)
        return 1;
    return 0;
}
