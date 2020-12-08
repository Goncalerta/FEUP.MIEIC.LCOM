#include "textbox.h"

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

static char *clip_board = NULL; // not saving the '\0' char
static uint8_t clip_board_size = 0;

void new_text_box(text_box_t *text_box, uint16_t x, uint16_t y, uint8_t display_size) {
    text_box->word = malloc(sizeof('\0'));
    text_box->word[0] = '\0';
    text_box->word_size = 0;
    text_box->cursor_pos = 0;
    text_box->select_pos = 0;
    text_box->start_display = 0;
    text_box->state = TEXT_BOX_NORMAL;

    text_box->x = x;
    text_box->y = y;
    text_box->display_size = display_size;
}

int text_box_draw(frame_buffer_t buf, text_box_t text_box, bool is_cursor_to_draw) {
    uint32_t text_box_color = text_box.state == TEXT_BOX_NORMAL ? TEXT_BOX_NORMAL_COLOR : TEXT_BOX_HOVERING_COLOR;

    if (vb_draw_rectangle(buf, text_box.x, text_box.y, TEXT_BOX_WIDTH(text_box.display_size), TEXT_BOX_HEIGHT, text_box_color) != 0) {
        printf("Error printing the text_box\n");
        return 1;
    }

    if (text_box.state == TEXT_BOX_SELECTED) { // draw boarder
        if (vb_draw_hline(buf, text_box.x, text_box.y, TEXT_BOX_WIDTH(text_box.display_size), TEXT_BOX_BORDER_COLOR) != 0)
            return 1;
        if (vb_draw_hline(buf, text_box.x, text_box.y + TEXT_BOX_HEIGHT - 1, TEXT_BOX_WIDTH(text_box.display_size), TEXT_BOX_BORDER_COLOR) != 0)
            return 1;
        if (vb_draw_vline(buf, text_box.x, text_box.y, TEXT_BOX_HEIGHT, TEXT_BOX_BORDER_COLOR) != 0)
            return 1;
        if (vb_draw_vline(buf, text_box.x + TEXT_BOX_WIDTH(text_box.display_size) - 1, text_box.y, TEXT_BOX_HEIGHT, TEXT_BOX_BORDER_COLOR) != 0)
            return 1;
    }
    
    if (text_box.cursor_pos != text_box.select_pos) { // highlight
        int start = text_box.cursor_pos < text_box.select_pos ? text_box.cursor_pos : text_box.select_pos; // relative to string
        int end = text_box.cursor_pos > text_box.select_pos ? text_box.cursor_pos : text_box.select_pos; // retlative to string

        start -= text_box.start_display; // relative to text_box
        end -= text_box.start_display; // relative to text_box

        start = start < 0 ? 0 : start;
        end = end > text_box.display_size ? text_box.display_size : end;
        
        if (vb_draw_rectangle(buf, text_box.x + TEXT_BOX_BEG_END_SPACE + start*CHAR_SPACE, 
            text_box.y + TEXT_BOX_TOP_BOT_SPACE - (TEXT_BOX_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2, 
            (end - start)*CHAR_SPACE, TEXT_BOX_CURSOR_HEIGHT, TEXT_BOX_HIGHLIGHTED_TEXT_COLOR) != 0) {
            printf("Error highlighting the text_box\n");
            return 1;
        }
    }
    
    if (font_draw_string(buf, text_box.word, text_box.x + TEXT_BOX_BEG_END_SPACE, 
        text_box.y + TEXT_BOX_TOP_BOT_SPACE, text_box.start_display, text_box.display_size) != 0) {
        return 1;
    }

    if (is_cursor_to_draw && text_box.state == TEXT_BOX_SELECTED) {
        uint16_t cursor_x = text_box.x + TEXT_BOX_BEG_END_SPACE + (text_box.cursor_pos - text_box.start_display)*CHAR_SPACE;
        uint16_t cursor_y = text_box.y + TEXT_BOX_TOP_BOT_SPACE - (TEXT_BOX_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2;

        cursor_x = cursor_x > (text_box.x + TEXT_BOX_BEG_END_SPACE + CHAR_SPACE * text_box.display_size) ? text_box.x + TEXT_BOX_BEG_END_SPACE + CHAR_SPACE * text_box.display_size : cursor_x;

        if (vb_draw_vline(buf, cursor_x, cursor_y, TEXT_BOX_CURSOR_HEIGHT, TEXT_BOX_CURSOR_COLOR) != 0) {
            return 1;
        }
    }
    return 0;
}

bool text_box_is_hovering(text_box_t text_box, uint16_t x, uint16_t y) {
    return x >= text_box.x && y >= text_box.y 
        && x < text_box.x + TEXT_BOX_WIDTH(text_box.display_size) 
        && y < text_box.y + TEXT_BOX_HEIGHT;
}

int text_box_update_state(text_box_t *text_box, bool hovering, bool lb, bool rb) {
    switch (text_box->state) {
    case TEXT_BOX_NORMAL:
        if (hovering && !(lb || rb)) {
            text_box->state = TEXT_BOX_HOVERING;
        }
        break;

    case TEXT_BOX_HOVERING:
        if (hovering) {
            if (lb && !rb) {
                text_box->state = TEXT_BOX_SELECTED;
            }
        } else {
            text_box->state = TEXT_BOX_NORMAL;
        }
        break;

    case TEXT_BOX_SELECTED:
        if (!hovering) {
            text_box->state = TEXT_BOX_NORMAL;
        }
        break;
    }
    return 0;
}

int text_box_react_kbd(text_box_t *text_box, kbd_state kbd_event) {
    if (text_box->state != TEXT_BOX_SELECTED) {
        return 0;
    }
    // TODO ainda falta ir ajustando que parte da palavra fica visivel na text_box
    switch (kbd_event.key) {
    case CHAR:
        if (kbd_is_ctrl_pressed()) {
            switch (kbd_event.char_key) {
            case 'C':
                if (text_box->cursor_pos != text_box->select_pos) {
                    uint8_t from = text_box->cursor_pos < text_box->select_pos ? text_box->cursor_pos : text_box->select_pos;
                    uint8_t to = text_box->cursor_pos > text_box->select_pos ? text_box->cursor_pos : text_box->select_pos;

                    if (clip_board == NULL) {
                        clip_board = malloc((to-from)*sizeof(char)); // TODO é para verificar return the NULL to malloc/realloc?
                    } else {
                        clip_board = realloc(clip_board, (to-from)*sizeof(char));
                    }
                    if (memcpy(clip_board, text_box->word+from, to-from) == NULL) {
                        printf("Error while CTRL+C\n");
                        return 1;
                    }
                    clip_board_size = to - from;
                } 
                break;
            
            case 'V':
                if (text_box->cursor_pos != text_box->select_pos) {
                    //TODO
                } else {
                    text_box->word = realloc(text_box->word, text_box->word_size + clip_board_size + 1);
                    if (memmove(text_box->word + text_box->cursor_pos+clip_board_size, text_box->word + text_box->cursor_pos, text_box->word_size-text_box->cursor_pos+1) == NULL) {
                        printf("Error while CTRL+V\n");
                        return 1;
                    }
                    if (memmove(text_box->word + text_box->cursor_pos, clip_board, clip_board_size) == NULL) {
                        printf("Error while CTRL+V\n");
                        return 1;
                    }
                    text_box->cursor_pos += clip_board_size;
                    text_box->select_pos = text_box->cursor_pos;
                    text_box->word_size += clip_board_size;
                    // TODO adjust start_display if needed
                }
                break;
            }
        } else {
            if (text_box->cursor_pos != text_box->select_pos) {
                //TODO
            } else {
                text_box->word = realloc(text_box->word, text_box->word_size + 2); // 2 = 1 + 1 ('\0' + new char)
                if (memmove(text_box->word + text_box->cursor_pos+1, text_box->word + text_box->cursor_pos, text_box->word_size-text_box->cursor_pos+1) == NULL) {
                    printf("Error while writing\n");
                    return 1;
                }
                text_box->word[text_box->cursor_pos++] = kbd_event.char_key;
                text_box->select_pos = text_box->cursor_pos;
                text_box->word_size++;
                // TODO adjust start_display if needed
            }
        }
        break;
    
    case BACK_SPACE:
        if (text_box->cursor_pos == 0) {
            return 0;
        }

        if (text_box->cursor_pos != text_box->select_pos) {
            //TODO
        } else {
            text_box->word = realloc(text_box->word, text_box->word_size);
            if (memmove(text_box->word + text_box->cursor_pos-1, text_box->word + text_box->cursor_pos, text_box->word_size-text_box->cursor_pos+1) == NULL) {
                printf("Error while deleting\n");
                return 1;
            }
            text_box->cursor_pos--;
            text_box->select_pos = text_box->cursor_pos;
            text_box->word_size--;
            // TODO adjust start_display if needed
        }
        break;
    
    case DEL:
        if (text_box->cursor_pos == text_box->word_size) {
            return 0;
        }

        if (text_box->cursor_pos != text_box->select_pos) {
            //TODO
        } else {
            text_box->word = realloc(text_box->word, text_box->word_size);
            if (memmove(text_box->word + text_box->cursor_pos, text_box->word + text_box->cursor_pos+1, text_box->word_size-text_box->cursor_pos+1) == NULL) {
                printf("Error while deleting\n");
                return 1;
            }
            text_box->word_size--;
            // TODO adjust start_display if needed
        }
        break;
    
    case ARROW_LEFT:
        if (text_box->cursor_pos > 0) {
            text_box->cursor_pos--;
        }
        if (!kbd_is_ctrl_pressed()) {
            text_box->select_pos = text_box->cursor_pos;
        }
        if (text_box->cursor_pos < text_box->start_display) {
            text_box->start_display--;
        }
        break;

    case ARROW_RIGHT:
        if (text_box->cursor_pos < text_box->word_size) {
            text_box->cursor_pos++;
        }
        if (!kbd_is_ctrl_pressed()) {
            text_box->select_pos = text_box->cursor_pos;
        }
        if (text_box->cursor_pos > text_box->start_display + text_box->display_size) {
            text_box->start_display++;
        }
        break;
    
    case ENTER:
        // TODO forwards the word to somewhere (?)
        break;
    
    default:
        break;
    }
    
    return 0;
}
