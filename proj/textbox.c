#include "textbox.h"

// static char *clip_board;

void new_text_box(text_box_t *text_box, uint16_t x, uint16_t y, uint8_t display_size) {
    text_box->word = "";
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
    // TODO
    if (kbd_is_ctrl_pressed()) {
        switch (kbd_event.key) {
        case CHAR:
            switch (kbd_event.char_key) {
            case 'C':
                
                break;
            
            case 'V':

                break;
            }
            break;

        case ARROW_LEFT:

            break;

        case ARROW_RIGHT:

            break;
        
        default:
            break;
        }
    } else {
        switch (kbd_event.key) {
        case CHAR:
            
            break;
        
        case SPACE:

            break;
        
        case BACK_SPACE:

            break;
        
        case DEL:

            break;
        
        case ARROW_LEFT:

            break;

        case ARROW_RIGHT:

            break;
        
        case ENTER:

            break;
        
        default:
            break;
        }
    }

    return 0;
}
