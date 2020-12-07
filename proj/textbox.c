#include "textbox.h"

static text_box text_box_guesser; // TODO array de text_boxes para poder selecionar a que está selected
                                  // isto porque só vai estar selecionada no máximo uma
                                  // not sure se é boa idea fazer assim
// ...
// static char *clip_board;

static text_box* text_box_search(text_boxes tex_box) {
    switch (tex_box) {
    case GUESSER:
        return &text_box_guesser;
        break;
    }
}

void text_box_initiate(text_boxes tex_box) {
    text_box *t_box = text_box_search(tex_box);

    t_box->word = "";
    t_box->cursor_pos = 0;
    t_box->select_pos = 0;
    t_box->start_display = 0;
    t_box->is_selected = false;

    switch (tex_box) {
    case GUESSER:
        text_box_guesser.x = TEXT_BOX_GUESS_X;
        text_box_guesser.y = TEXT_BOX_GUESS_Y;
        text_box_guesser.display_size = TEXT_BOX_GUESS_DISPLAY_SIZE;
        break;
    }
}

int text_box_draw(frame_buffer_t buf, text_boxes tex_box, bool is_cursor_to_draw) {
    text_box *t_box = text_box_search(tex_box);

    if (vb_draw_rectangle(buf, t_box->x, t_box->y, TEXT_BOX_WIDTH(t_box->display_size), TEXT_BOX_HEIGHT, COLOR_TEXT_BOX) != 0) {
        printf("Error printing the text_box\n");
        return 1;
    }

    if (t_box->is_selected) { // draw boarder
        if (vb_draw_hline(buf, t_box->x, t_box->y, TEXT_BOX_WIDTH(t_box->display_size), COLOR_TEXT_BOX_BORDER) != 0)
            return 1;
        if (vb_draw_hline(buf, t_box->x, t_box->y + TEXT_BOX_HEIGHT - 1, TEXT_BOX_WIDTH(t_box->display_size), COLOR_TEXT_BOX_BORDER) != 0)
            return 1;
        if (vb_draw_vline(buf, t_box->x, t_box->y, TEXT_BOX_HEIGHT, COLOR_TEXT_BOX_BORDER) != 0)
            return 1;
        if (vb_draw_vline(buf, t_box->x + TEXT_BOX_WIDTH(t_box->display_size) - 1, t_box->y, TEXT_BOX_HEIGHT, COLOR_TEXT_BOX_BORDER) != 0)
            return 1;
    }
    
    if (t_box->cursor_pos != t_box->select_pos) { // highlight
        int start = t_box->cursor_pos < t_box->select_pos ? t_box->cursor_pos : t_box->select_pos; // relative to string
        int end = t_box->cursor_pos > t_box->select_pos ? t_box->cursor_pos : t_box->select_pos; // retlative to string

        start -= t_box->start_display; // relative to text_box
        end -= t_box->start_display; // relative to text_box

        start = start < 0 ? 0 : start;
        end = end > t_box->display_size ? t_box->display_size : end;
        
        if (vb_draw_rectangle(buf, t_box->x + TEXT_BOX_BEG_END_SPACE + start*CHAR_SPACE, 
            t_box->y + TEXT_BOX_TOP_BOT_SPACE - (FONT_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2, 
            (end - start)*CHAR_SPACE, FONT_CURSOR_HEIGHT, COLOR_HIGHLIGHTED_TEXT) != 0) {
            printf("Error highlighting the text_box\n");
            return 1;
        }
    }
    
    if (font_draw_string(buf, t_box->word, t_box->x + TEXT_BOX_BEG_END_SPACE, 
        t_box->y + TEXT_BOX_TOP_BOT_SPACE, t_box->start_display, t_box->display_size) != 0) {
        return 1;
    }

    if (is_cursor_to_draw) {
        uint16_t cursor_x = t_box->x + TEXT_BOX_BEG_END_SPACE + (t_box->cursor_pos - t_box->start_display)*CHAR_SPACE;
        uint16_t cursor_y = t_box->y + TEXT_BOX_TOP_BOT_SPACE - (FONT_CURSOR_HEIGHT - FONT_CHAR_HEIGHT)/2;

        cursor_x = cursor_x > (t_box->x + TEXT_BOX_BEG_END_SPACE + CHAR_SPACE * t_box->display_size) ? t_box->x + TEXT_BOX_BEG_END_SPACE + CHAR_SPACE * t_box->display_size : cursor_x;

        if (font_draw_cursor(buf, cursor_x, cursor_y) != 0) {
            return 1;
        }
    }
    return 0;
}

void text_box_select(text_boxes tex_box) {
    text_box *t_box = text_box_search(tex_box);
    t_box->is_selected = true;
}

void text_box_unselect(text_boxes tex_box) {
    text_box *t_box = text_box_search(tex_box);
    t_box->is_selected = false;
}

bool is_text_box_selected(text_boxes tex_box) {
    text_box *t_box = text_box_search(tex_box);
    return t_box->is_selected;
}

void text_box_react(text_boxes tex_box, kbd_state kbd_event) {
    text_box *t_box = text_box_search(tex_box);
    if (!t_box->is_selected) {
        return;
    }
    
    // TODO
    // + falta ter codes para o SHIFT
}
