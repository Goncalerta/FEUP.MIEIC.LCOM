#ifndef __TEXTBOX_H
#define __TEXTBOX_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include "font.h"
#include "keyboard.h"

#define TEXT_BOX_CURSOR_HEIGHT 22
#define TEXT_BOX_CURSOR_COLOR 0x999999

#define TEXT_BOX_BEG_END_SPACE 4
#define TEXT_BOX_TOP_BOT_SPACE 8
#define TEXT_BOX_HEIGHT (FONT_CHAR_HEIGHT + 2*TEXT_BOX_TOP_BOT_SPACE)
#define TEXT_BOX_WIDTH(display_size) ((display_size) * CHAR_SPACE + 2*TEXT_BOX_BEG_END_SPACE)

#define TEXT_BOX_HIGHLIGHTED_TEXT_COLOR 0xaaffff //TODO adicionar header com #defines de cores (?)
#define TEXT_BOX_NORMAL_COLOR 0xf7f7f7
#define TEXT_BOX_HOVERING_COLOR 0xffffff
#define TEXT_BOX_BORDER_COLOR 0x0000aa

#define TEXT_BOX_GUESSER_X 30 //TODO ajustar para o certo
#define TEXT_BOX_GUESSER_Y 700 //TODO ajustar para o certo
#define TEXT_BOX_GUESSER_DISPLAY_SIZE 13 //TODO acertar se necessário


typedef enum text_box_state {
    TEXT_BOX_NORMAL,
    TEXT_BOX_HOVERING,
    TEXT_BOX_SELECTED
} text_box_state;

typedef struct text_box_t {
    char *   word;
    uint16_t x, y;
    uint8_t  cursor_pos;    // relative to the string
    uint8_t  select_pos;    // relative to the string // used to select letters with mouse
    uint8_t  start_display; // first char pos displayed in the text_box
    uint8_t  display_size;  // num of chars displayed at the same time
    text_box_state state;
} text_box_t;


void new_text_box(text_box_t *text_box, uint16_t x, uint16_t y, uint8_t display_size);

/* is_cursor_to_draw is ised to coordinate with the game ticks
if text_box is not selected, the cursor is not drawn*/
int text_box_draw(frame_buffer_t buf, text_box_t text_box, bool is_cursor_to_draw);

bool text_box_is_hovering(text_box_t text_box, uint16_t x, uint16_t y);

int text_box_update_state(text_box_t *text_box, bool hovering, bool lb, bool rb);

int text_box_react_kbd(text_box_t *text_box, kbd_state kbd_event);

// TODO text_box_clear using free()

#endif /* __TEXTBOX_H */
