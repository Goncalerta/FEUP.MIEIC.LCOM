#ifndef __TEXTBOX_H
#define __TEXTBOX_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include "font.h"
#include "keyboard.h"


#define TEXT_BOX_BEG_END_SPACE 4
#define TEXT_BOX_TOP_BOT_SPACE 8
#define TEXT_BOX_HEIGHT (FONT_CHAR_HEIGHT + 2*TEXT_BOX_TOP_BOT_SPACE)
#define TEXT_BOX_WIDTH(display_size) ((display_size) * CHAR_SPACE + 2*TEXT_BOX_BEG_END_SPACE)

#define TEXT_BOX_GUESS_X 30 //TODO ajustar para o certo
#define TEXT_BOX_GUESS_Y 680 //TODO ajustar para o certo
#define TEXT_BOX_GUESS_DISPLAY_SIZE 13 //TODO acertar se necessário

#define COLOR_HIGHLIGHTED_TEXT 0xaaffff //TODO adicionar header com #defines de cores (?)
#define COLOR_TEXT_BOX 0xeeeeee
#define COLOR_TEXT_BOX_BORDER 0x0000aa

typedef struct text_box {
    char *   word;
    uint16_t x, y;
    uint8_t  cursor_pos; // relative to the string
    uint8_t  select_pos; // relative to the string // used to select letters with mouse
    uint8_t  start_display; // first char pos displayed in the text_box
    uint8_t  display_size; // num of chars displayed at the same time
    bool     is_selected;
} text_box;

typedef enum text_boxes {
    GUESSER
} text_boxes;

void text_box_initiate(text_boxes tex_box);

int text_box_draw(frame_buffer_t buf, text_boxes tex_box, bool is_cursor_to_draw);

void text_box_select(text_boxes tex_box);

void text_box_unselect(text_boxes tex_box);

bool is_text_box_selected(text_boxes tex_box);

void text_box_react(text_boxes tex_box, kbd_state kbd_event);

// TODO text_box_clear using free()

#endif /* __TEXTBOX_H */
