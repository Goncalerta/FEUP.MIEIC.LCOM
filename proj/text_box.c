#include <lcom/lcf.h>
#include "text_box.h"
#include "font.h"

#define TEXT_BOX_CURSOR_HEIGHT (FONT_CHAR_HEIGHT + 4)
#define TEXT_BOX_CURSOR_COLOR 0x000000

#define TEXT_BOX_BEG_END_SPACE 4
#define TEXT_BOX_TOP_BOT_SPACE 8
#define TEXT_BOX_HEIGHT (FONT_CHAR_HEIGHT + 2*TEXT_BOX_TOP_BOT_SPACE)
#define TEXT_BOX_WIDTH(display_size) ((display_size) * CHAR_SPACE + 2*TEXT_BOX_BEG_END_SPACE)

#define TEXT_BOX_HIGHLIGHTED_TEXT_COLOR 0xaaffff
#define TEXT_BOX_NORMAL_COLOR 0xf7f7f7
#define TEXT_BOX_HOVERING_COLOR 0xffffff
#define TEXT_BOX_BORDER_COLOR 0x0000aa

// Enumerated type for specifying the state of a text box.
typedef enum text_box_state {
    TEXT_BOX_NORMAL, // Text box is in normal/base state.
    TEXT_BOX_HOVERING, // Text box is not selected and the cursor is hovering it.
    TEXT_BOX_SELECTED_HOVERING, // Text box is selected and the cursor is hovering it.
    TEXT_BOX_SELECTED_NOT_HOVERING, // Text box is selected but the cursor is not hovering it.
    TEXT_BOX_PRESSING // Text box is being pressed.
} text_box_state_t;

// Text box class implementation.
struct text_box {
    char *word; // Address of memory of the content of the text box.
    uint8_t word_size; // Content size (not counting with the '\0').
    uint16_t x; // Left most x coordinate of the text box.
    uint16_t y; // Top most y coordinate of the text box.
    uint8_t cursor_pos; // Cursor position relative to its content start.
    uint8_t select_pos; // Position, relative to content start, from where the content is being selected (== cursor_pos if nothing selected).
    uint8_t start_display; // First position dispalyed in the text box.
    uint8_t display_size; // Number of chars displayed at the same time.
    text_box_state_t state; // State of the text box.
    bool visible_cursor; // True if the text cursor is visible and false otherwise.
    text_box_action_t action; // Action to perform when ENTER is pressed.
};

static char *clip_board = NULL; // not saving the '\0' char
static uint8_t clip_board_size = 0;

text_box_t *new_text_box(uint16_t x, uint16_t y, uint8_t display_size, text_box_action_t action) {
    text_box_t *text_box = malloc(sizeof(text_box_t));
    if (text_box == NULL)
        return NULL;
    
    text_box->word = malloc(sizeof('\0'));
    if (text_box->word == NULL)
        return NULL;
    text_box->word[0] = '\0';
    text_box->word_size = 0;
    text_box->cursor_pos = 0;
    text_box->select_pos = 0;
    text_box->start_display = 0;
    text_box->state = TEXT_BOX_NORMAL;
    text_box->visible_cursor = true;
    text_box->action = action;

    text_box->x = x;
    text_box->y = y;
    text_box->display_size = display_size;

    return text_box;
}

bool text_box_is_reacting_to_cursor_hovering(text_box_t *text_box) {
    return text_box->state != TEXT_BOX_NORMAL && text_box->state != TEXT_BOX_SELECTED_NOT_HOVERING;
}

void text_box_cursor_tick(text_box_t *text_box) {
    text_box->visible_cursor = !text_box->visible_cursor;
}

int text_box_clear(text_box_t *text_box) {
    char *word = realloc(text_box->word, sizeof('\0'));
    if (word == NULL)
        return 1;
    
    text_box->word = word;

    text_box->word[0] = '\0';
    text_box->word_size = 0;
    text_box->cursor_pos = 0;
    text_box->select_pos = 0;
    text_box->start_display = 0;

    return 0;
}

int text_box_draw(frame_buffer_t buf, text_box_t *text_box) {
    uint32_t text_box_color = text_box->state == TEXT_BOX_NORMAL ? TEXT_BOX_NORMAL_COLOR : TEXT_BOX_HOVERING_COLOR;
    bool text_box_interacting = text_box->state == TEXT_BOX_SELECTED_HOVERING || text_box->state == TEXT_BOX_SELECTED_NOT_HOVERING || text_box->state == TEXT_BOX_PRESSING;

    if (vb_draw_rectangle(buf, text_box->x, text_box->y, TEXT_BOX_WIDTH(text_box->display_size), TEXT_BOX_HEIGHT, text_box_color) != OK) {
        printf("Error printing the text_box\n");
        return 1;
    }

    if (text_box_interacting) { // draw border
        if (vb_draw_hline(buf, text_box->x, text_box->y, TEXT_BOX_WIDTH(text_box->display_size), TEXT_BOX_BORDER_COLOR) != OK)
            return 1;
        if (vb_draw_hline(buf, text_box->x, text_box->y + TEXT_BOX_HEIGHT - 1, TEXT_BOX_WIDTH(text_box->display_size), TEXT_BOX_BORDER_COLOR) != OK)
            return 1;
        if (vb_draw_vline(buf, text_box->x, text_box->y, TEXT_BOX_HEIGHT, TEXT_BOX_BORDER_COLOR) != OK)
            return 1;
        if (vb_draw_vline(buf, text_box->x + TEXT_BOX_WIDTH(text_box->display_size) - 1, text_box->y, TEXT_BOX_HEIGHT, TEXT_BOX_BORDER_COLOR) != OK)
            return 1;
    }
    
    if (text_box->cursor_pos != text_box->select_pos && text_box_interacting) { // highlight
        int start = text_box->cursor_pos < text_box->select_pos ? text_box->cursor_pos : text_box->select_pos; // relative to string
        int end = text_box->cursor_pos > text_box->select_pos ? text_box->cursor_pos : text_box->select_pos; // retlative to string

        start -= text_box->start_display; // relative to text_box
        end -= text_box->start_display; // relative to text_box

        start = start < 0 ? 0 : start;
        end = end > text_box->display_size ? text_box->display_size : end;
        
        if (vb_draw_rectangle(buf, text_box->x + TEXT_BOX_BEG_END_SPACE + start*CHAR_SPACE, 
            text_box->y + TEXT_BOX_TOP_BOT_SPACE - (TEXT_BOX_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2, 
            (end - start)*CHAR_SPACE, TEXT_BOX_CURSOR_HEIGHT, TEXT_BOX_HIGHLIGHTED_TEXT_COLOR) != OK) {
            printf("Error highlighting the text_box\n");
            return 1;
        }
    }
    
    if (font_draw_string_limited(buf, text_box->word, text_box->x + TEXT_BOX_BEG_END_SPACE, 
        text_box->y + TEXT_BOX_TOP_BOT_SPACE, text_box->start_display, text_box->display_size) != OK) {
        return 1;
    }

    if (text_box->visible_cursor && text_box_interacting) { // cursor
        uint16_t cursor_x = text_box->x + TEXT_BOX_BEG_END_SPACE + (text_box->cursor_pos - text_box->start_display)*CHAR_SPACE;
        uint16_t cursor_y = text_box->y + TEXT_BOX_TOP_BOT_SPACE - (TEXT_BOX_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2;

        cursor_x = cursor_x > (text_box->x + TEXT_BOX_BEG_END_SPACE + CHAR_SPACE * text_box->display_size) ? text_box->x + TEXT_BOX_BEG_END_SPACE + CHAR_SPACE * text_box->display_size : cursor_x;

        if (vb_draw_vline(buf, cursor_x, cursor_y, TEXT_BOX_CURSOR_HEIGHT, TEXT_BOX_CURSOR_COLOR) != OK) {
            return 1;
        }
    }
    return 0;
}

bool text_box_is_hovering(text_box_t *text_box, uint16_t x, uint16_t y) {
    return x >= text_box->x && y >= text_box->y 
        && x < text_box->x + TEXT_BOX_WIDTH(text_box->display_size) 
        && y < text_box->y + TEXT_BOX_HEIGHT;
}

void text_box_unselect(text_box_t *text_box) {
    text_box->state = TEXT_BOX_NORMAL;
}

int text_box_update_state(text_box_t *text_box, bool hovering, bool lb, bool rb, uint16_t x, uint16_t y) {
    int8_t mouse_pos = (x - text_box->x)/CHAR_SPACE;
    mouse_pos += text_box->start_display;

    // adjustments
    if (mouse_pos > text_box->word_size) {
        mouse_pos = text_box->word_size;
    } else if (mouse_pos < 0) {
        mouse_pos = 0;
    }

    if (lb && !rb) {
        text_box->cursor_pos = mouse_pos;
        text_box->visible_cursor = true;
    }

    switch (text_box->state) {
    case TEXT_BOX_NORMAL:
        if (hovering && !(lb || rb)) {
            text_box->state = TEXT_BOX_HOVERING;
        }
        break;

    case TEXT_BOX_HOVERING:
        if (hovering) {
            if (lb && !rb) {
                text_box->state = TEXT_BOX_SELECTED_HOVERING;
                text_box->select_pos = text_box->cursor_pos;
            }
        } else {
            text_box->state = TEXT_BOX_NORMAL;
        }
        break;

    case TEXT_BOX_SELECTED_HOVERING:
        if (hovering) {
           if (lb && !rb) {
                text_box->state = TEXT_BOX_PRESSING;
                text_box->select_pos = text_box->cursor_pos;
            }
        } else {
            text_box->state = TEXT_BOX_SELECTED_NOT_HOVERING;
        }
        break;
    
    case TEXT_BOX_SELECTED_NOT_HOVERING:
        if (hovering) {
            text_box->state = TEXT_BOX_SELECTED_HOVERING;
        } else if (lb || rb) {
            text_box->state = TEXT_BOX_NORMAL;
        }
        break;

    case TEXT_BOX_PRESSING:
        if (!(lb || rb)) {
            if (hovering)
                text_box->state = TEXT_BOX_SELECTED_HOVERING;
            else
                text_box->state = TEXT_BOX_SELECTED_NOT_HOVERING;
        }

        if (text_box->cursor_pos <= text_box->start_display && text_box->start_display > 0) {
            text_box->start_display = text_box->cursor_pos == 0 ? 0 : text_box->cursor_pos - 1;
        } else if (text_box->cursor_pos >= text_box->start_display + text_box->display_size) {
            text_box->start_display = text_box->cursor_pos - text_box->display_size + 1;
        }
        break;
    }
    return 0;
}

static int text_box_delete_selected(text_box_t *text_box) {
    if (text_box->cursor_pos == text_box->select_pos) {
        return 0;
    }
    
    uint8_t from = text_box->cursor_pos < text_box->select_pos ? text_box->cursor_pos : text_box->select_pos;
    uint8_t to = text_box->cursor_pos > text_box->select_pos ? text_box->cursor_pos : text_box->select_pos;

    if (memmove(text_box->word + from, text_box->word + to, text_box->word_size-from+1) == NULL) {
        printf("Error while deleting selected\n");
        return 1;
    }

    char *word = realloc(text_box->word, text_box->word_size + (to-from) + 1);
    if (word == NULL)
        return 1;
    
    text_box->word = word;
    
    text_box->cursor_pos = text_box->select_pos = from;
    text_box->word_size -= (to-from);

    return 0;
}

static int text_box_copy(text_box_t *text_box) {
    if (text_box->cursor_pos == text_box->select_pos) {
        return 0;
    }

    uint8_t from = text_box->cursor_pos < text_box->select_pos ? text_box->cursor_pos : text_box->select_pos;
    uint8_t to = text_box->cursor_pos > text_box->select_pos ? text_box->cursor_pos : text_box->select_pos;

    if (clip_board == NULL) {
        clip_board = malloc((to-from)*sizeof(char));
        if (clip_board == NULL)
            return 1;
    } else {
        char *clip_board_temp = realloc(clip_board, (to-from)*sizeof(char));
        if (clip_board_temp == NULL)
            return 1;
        clip_board = clip_board_temp;
    }
    if (memcpy(clip_board, text_box->word+from, to-from) == NULL) {
        printf("Error while copying\n");
        return 1;
    }
    clip_board_size = to - from;

    return 0;
}

static int text_box_retrieve(text_box_t *text_box) {
    char *content = malloc(text_box->word_size + 1);
    if (content == NULL)
        return 1;

    if (memcpy(content, text_box->word, text_box->word_size + 1) == NULL) {
        printf("Error while retrieving word\n");
        return 1;
    }

    if (text_box->action(content) != OK)
        return 1;
    
    if (text_box_clear(text_box) != OK)
        return 1;

    return 0;
}

int text_box_react_kbd(text_box_t *text_box, kbd_event_t kbd_event) {
    if (text_box->state != TEXT_BOX_SELECTED_HOVERING && text_box->state != TEXT_BOX_SELECTED_NOT_HOVERING) {
        return 0;
    }

    text_box->visible_cursor = true;

    char *word; // Used for realloc
    switch (kbd_event.key) {
    case CHAR:
        if (kbd_event.is_ctrl_pressed) {
            switch (kbd_event.char_key) {
            case 'C':
                if (text_box_copy(text_box) != OK) {
                    return 1;
                }
                break;
            
            case 'V':
                if (text_box->cursor_pos != text_box->select_pos) {
                    if (text_box_delete_selected(text_box) != OK) {
                        return 1;
                    }
                }

                word = realloc(text_box->word, text_box->word_size + clip_board_size + 1);
                if (word == NULL)
                    return 1;
                
                text_box->word = word;
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
                break;

            case 'X':
                if (text_box_copy(text_box) != OK) {
                    return 1;
                }
                if (text_box_delete_selected(text_box) != OK) {
                    return 1;
                }
                break;
            }
        } else {
            if (text_box->cursor_pos != text_box->select_pos) {
                if (text_box_delete_selected(text_box) != OK) {
                    return 1;
                }
            }

            word = realloc(text_box->word, text_box->word_size + 2); // 2 = 1 + 1 ('\0' + new char)
            if (word == NULL)
                return 1;
            
            text_box->word = word;
            if (memmove(text_box->word + text_box->cursor_pos+1, text_box->word + text_box->cursor_pos, text_box->word_size-text_box->cursor_pos+1) == NULL) {
                printf("Error while writing\n");
                return 1;
            }
            text_box->word[text_box->cursor_pos++] = kbd_event.char_key;
            text_box->select_pos = text_box->cursor_pos;
            text_box->word_size++;
        }
        break;
    
    case BACK_SPACE:
        if (text_box->cursor_pos != text_box->select_pos) {
            if (text_box_delete_selected(text_box) != OK) {
                return 1;
            }
        } else {
            if (text_box->cursor_pos == 0) {
                return 0;
            }
            
            if (memmove(text_box->word + text_box->cursor_pos-1, text_box->word + text_box->cursor_pos, text_box->word_size-text_box->cursor_pos+1) == NULL) {
                printf("Error while deleting\n");
                return 1;
            }
            
            word = realloc(text_box->word, text_box->word_size);
            if (word == NULL)
                return 1;
            
            text_box->word = word;

            if (text_box->start_display > 0) {
                text_box->start_display--;
            }

            text_box->cursor_pos--;
            text_box->select_pos = text_box->cursor_pos;
            text_box->word_size--;
        }
        break;
    
    case DEL:
        if (text_box->cursor_pos == text_box->word_size) {
            return 0;
        }

        if (text_box->cursor_pos != text_box->select_pos) {
            if (text_box_delete_selected(text_box) != OK) {
                return 1;
            }
        } else {
            if (memmove(text_box->word + text_box->cursor_pos, text_box->word + text_box->cursor_pos+1, text_box->word_size-text_box->cursor_pos+1) == NULL) {
                printf("Error while deleting\n");
                return 1;
            }
            word = realloc(text_box->word, text_box->word_size);
            if (word == NULL)
                return 1;
            
            text_box->word = word;

            text_box->word_size--;
        }
        break;
    
    case ARROW_LEFT:
        if (text_box->cursor_pos > 0) {
            text_box->cursor_pos--;
        }
        if (!kbd_event.is_ctrl_pressed) {
            text_box->select_pos = text_box->cursor_pos;
        }
        break;

    case ARROW_RIGHT:
        if (text_box->cursor_pos < text_box->word_size) {
            text_box->cursor_pos++;
        }
        if (!kbd_event.is_ctrl_pressed) {
            text_box->select_pos = text_box->cursor_pos;
        }
        break;
    
    case ENTER:
        if (text_box_retrieve(text_box) != OK)
            return 1;
        break;
    
    default:
        break;
    }

    // adjusting the display
    if (text_box->cursor_pos > text_box->start_display + text_box->display_size) {
        text_box->start_display = text_box->cursor_pos - text_box->display_size;
    } else if (text_box->cursor_pos < text_box->start_display) {
        text_box->start_display = text_box->cursor_pos;
    }
    
    return 0;
}

void delete_text_box(text_box_t *text_box) {
    if (text_box == NULL)
        return;

    if (text_box->word != NULL) {
        free(text_box->word);
        text_box->word = NULL;
    }

    free(text_box);
}

void text_box_clip_board_exit() {
    if (clip_board == NULL) {
        return;
    }

    free(clip_board);
    clip_board = NULL;
}
