#include "textbox.h"

// static char *clip_board;

int text_box_draw(frame_buffer_t buf, text_box t_box, bool is_cursor_to_draw) {
    if (vb_draw_rectangle(buf, t_box.x_pos, t_box.y_pos, t_box.width, t_box.height, COLOR_TEXT_BOX_GUESS) != 0) {
        printf("Error printing the text_box\n");
        return 1;
    }

    if (t_box.cursor_pos != t_box.select_pos) { // highlight
        int start = t_box.cursor_pos < t_box.select_pos ? t_box.cursor_pos : t_box.select_pos; // relative to string
        int end = t_box.cursor_pos > t_box.select_pos ? t_box.cursor_pos : t_box.select_pos; // retlative to string

        start -= t_box.start_display; // relative to text_box
        end -= t_box.start_display; // relative to text_box

        start = start < 0 ? 0 : start;
        end = end > t_box.display_size ? t_box.display_size : end;
        
        if (vb_draw_rectangle(buf, t_box.x_pos + t_box.beg_end_space + start*CHAR_SPACE, 
            t_box.y_pos + t_box.top_bot_space - (FONT_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2, 
            (end - start)*CHAR_SPACE, FONT_CURSOR_HEIGHT, COLOR_HIGHLIGHTED_TEXT) != 0) {
            printf("Error highlighting the text_box\n");
            return 1;
        }
    }
    
    if (font_draw_string(buf, t_box.word, t_box.x_pos + t_box.beg_end_space, 
        t_box.y_pos + t_box.top_bot_space, t_box.start_display, t_box.display_size) != 0) {
        return 1;
    }

    if (is_cursor_to_draw) {
        uint16_t cursor_x = t_box.x_pos + t_box.beg_end_space + (t_box.cursor_pos - t_box.start_display)*CHAR_SPACE;
        uint16_t cursor_y = t_box.y_pos + t_box.top_bot_space - (FONT_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2;

        cursor_x = cursor_x > (t_box.x_pos + t_box.beg_end_space + CHAR_SPACE * t_box.display_size) ? t_box.x_pos + t_box.beg_end_space + CHAR_SPACE * t_box.display_size : cursor_x;

        if (font_draw_cursor(buf, cursor_x, cursor_y) != 0) {
            return 1;
        }
    }
    return 0;
}
