#include "textbox.h"

// static char *clip_board;

int text_box_draw(frame_buffer_t buf, text_box t_box, bool is_cursor_to_draw, bool is_selected) {
    if (vb_draw_rectangle(buf, t_box.x, t_box.y, TEXT_BOX_WIDTH(t_box.display_size), TEXT_BOX_HEIGHT, COLOR_TEXT_BOX) != 0) {
        printf("Error printing the text_box\n");
        return 1;
    }

    if (is_selected) { // draw boarder
        if (vb_draw_hline(buf, t_box.x, t_box.y, TEXT_BOX_WIDTH(t_box.display_size), COLOR_TEXT_BOX_BORDER) != 0)
            return 1;
        if (vb_draw_hline(buf, t_box.x, t_box.y + TEXT_BOX_HEIGHT - 1, TEXT_BOX_WIDTH(t_box.display_size), COLOR_TEXT_BOX_BORDER) != 0)
            return 1;
        if (vb_draw_vline(buf, t_box.x, t_box.y, TEXT_BOX_HEIGHT, COLOR_TEXT_BOX_BORDER) != 0)
            return 1;
        if (vb_draw_vline(buf, t_box.x + TEXT_BOX_WIDTH(t_box.display_size) - 1, t_box.y, TEXT_BOX_HEIGHT, COLOR_TEXT_BOX_BORDER) != 0)
            return 1;
    }
    
    if (t_box.cursor_pos != t_box.select_pos) { // highlight
        int start = t_box.cursor_pos < t_box.select_pos ? t_box.cursor_pos : t_box.select_pos; // relative to string
        int end = t_box.cursor_pos > t_box.select_pos ? t_box.cursor_pos : t_box.select_pos; // retlative to string

        start -= t_box.start_display; // relative to text_box
        end -= t_box.start_display; // relative to text_box

        start = start < 0 ? 0 : start;
        end = end > t_box.display_size ? t_box.display_size : end;
        
        if (vb_draw_rectangle(buf, t_box.x + TEXT_BOX_BEG_END_SPACE + start*CHAR_SPACE, 
            t_box.y + TEXT_BOX_TOP_BOT_SPACE - (FONT_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2, 
            (end - start)*CHAR_SPACE, FONT_CURSOR_HEIGHT, COLOR_HIGHLIGHTED_TEXT) != 0) {
            printf("Error highlighting the text_box\n");
            return 1;
        }
    }
    
    if (font_draw_string(buf, t_box.word, t_box.x + TEXT_BOX_BEG_END_SPACE, 
        t_box.y + TEXT_BOX_TOP_BOT_SPACE, t_box.start_display, t_box.display_size) != 0) {
        return 1;
    }

    if (is_cursor_to_draw) {
        uint16_t cursor_x = t_box.x + TEXT_BOX_BEG_END_SPACE + (t_box.cursor_pos - t_box.start_display)*CHAR_SPACE;
        uint16_t cursor_y = t_box.y + TEXT_BOX_TOP_BOT_SPACE - (FONT_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2;

        cursor_x = cursor_x > (t_box.x + TEXT_BOX_BEG_END_SPACE + CHAR_SPACE * t_box.display_size) ? t_box.x + TEXT_BOX_BEG_END_SPACE + CHAR_SPACE * t_box.display_size : cursor_x;

        if (font_draw_cursor(buf, cursor_x, cursor_y) != 0) {
            return 1;
        }
    }
    return 0;
}
