#ifndef __TEXTBOX_H
#define __TEXTBOX_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include "font.h"
#include "keyboard.h"

#define TEXT_BOX_GUESSER_X 30
#define TEXT_BOX_GUESSER_Y 700
#define TEXT_BOX_GUESSER_DISPLAY_SIZE 13

typedef int (*text_box_action)(char *);

typedef enum text_box_state {
    TEXT_BOX_NORMAL,
    TEXT_BOX_HOVERING,
    TEXT_BOX_SELECTED_HOVERING,
    TEXT_BOX_SELECTED_NOT_HOVERING,
    TEXT_BOX_PRESSING
} text_box_state;

typedef struct text_box_t {
    char *   word;
    uint8_t  word_size;     // not counting with the '\0'
    uint16_t x, y;
    uint8_t  cursor_pos;    // relative to the string
    uint8_t  select_pos;    // relative to the string // used to select letters with mouse
    uint8_t  start_display; // first char pos displayed in the text_box
    uint8_t  display_size;  // num of chars displayed at the same time
    text_box_state state;
    bool     visible_cursor;
    text_box_action action;
} text_box_t;


int new_text_box(text_box_t *text_box, uint16_t x, uint16_t y, uint8_t display_size, text_box_action action);

void delete_text_box(text_box_t *text_box);

int text_box_clear(text_box_t *text_box);

void text_box_cursor_tick(text_box_t *text_box);

int text_box_draw(frame_buffer_t buf, text_box_t text_box);

bool text_box_is_hovering(text_box_t text_box, uint16_t x, uint16_t y);

int text_box_update_state(text_box_t *text_box, bool hovering, bool lb, bool rb, uint16_t x, uint16_t y);

int text_box_react_kbd(text_box_t *text_box, kbd_event_t kbd_event);

void text_box_unselect(text_box_t *text_box);

void delete_text_box(text_box_t *text_box);

void text_box_clip_board_exit();

#endif /* __TEXTBOX_H */
