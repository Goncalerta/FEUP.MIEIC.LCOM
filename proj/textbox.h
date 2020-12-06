#ifndef __TEXTBOX_H
#define __TEXTBOX_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include "font.h"

#define TEXT_BOX_GUESS_X 10 //TODO ajustar para o certo
#define TEXT_BOX_GUESS_Y 10 //TODO ajustar para o certo
#define TEXT_BOX_GUESS_BEG_END_SPACE 4
#define TEXT_BOX_GUESS_TOP_BOT_SPACE 8
#define TEXT_BOX_GUESS_DISPLAY_SIZE 13 //TODO acertar se necessário
#define TEXT_BOX_GUESS_HEIGHT (FONT_CHAR_HEIGHT + 2*TEXT_BOX_GUESS_TOP_BOT_SPACE) 
#define TEXT_BOX_GUESS_WIDTH (TEXT_BOX_GUESS_DISPLAY_SIZE * CHAR_SPACE + 2*TEXT_BOX_GUESS_BEG_END_SPACE)

#define COLOR_HIGHLIGHTED_TEXT 0x00AAFFFF //TODO adiconar header com #defines de cores (?)
#define COLOR_TEXT_BOX_GUESS 0x00EEEEEE 

typedef struct text_box {
    char *   word;
    uint8_t  cursor_pos; // relative to the string
    uint8_t  select_pos; // relative to the string // used to select letters with mouse
    uint8_t  start_display; // first char pos displayed in the text_box
    uint16_t x_pos, y_pos;
    uint16_t height, width;
    uint8_t  display_size; // num of chars displayed at the same time
    uint8_t  beg_end_space, top_bot_space;
} text_box;

int text_box_draw(frame_buffer_t buf, text_box t_box, bool is_cursor_to_draw);


#endif /* __TEXTBOX_H */
