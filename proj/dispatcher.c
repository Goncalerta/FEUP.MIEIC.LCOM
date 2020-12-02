#include <lcom/lcf.h>
#include "dispatcher.h"
#include "kbc.h"
#include "keyboard.h"
#include "mouse.h"
#include "video_gr.h"
#include "canvas.h"
#include "cursor.h"
#include "font.h"

static bool end = false;

int dispatch_mouse_packet(struct packet p) {
    if (cursor_set_lb_state(p.lb)) {
        if (p.lb) {
            if (canvas_new_stroke(0x000033ff, 10) != OK)
                return 1;
        }
    }

    cursor_move(p.delta_x, p.delta_y);
    if (p.lb) {
        canvas_new_stroke_atom(cursor_get_x(), cursor_get_y());
    }


    return 0;
}

int dispatch_keyboard_event(KBD_STATE pressed_key) {
    // just to check if it's correct
    if (pressed_key.key == CHAR && !kbd_is_ctrl_pressed()) {
        if (font_draw_char(vg_get_back_buffer(), pressed_key.char_key, 10, 10) != 0) {
            printf("font_draw_char failed\n");
        }
    }
    if (pressed_key.key == ENTER) {
        char test_string[] = "TESTE 12";
        if (font_draw_string(vg_get_back_buffer(), test_string, 30, 10) != 0) {
            printf("font_draw_string failed\n");
        }
    }
    // ^^

    // just so I can test undo and redo without having to use mouse's middle button
    if (pressed_key.key == CHAR && pressed_key.char_key == 'Z' && kbd_is_ctrl_pressed()) {
        canvas_undo_stroke(); // no need to crash if empty
    }

    if (pressed_key.key == CHAR && pressed_key.char_key == 'Y' && kbd_is_ctrl_pressed()) {
        canvas_redo_stroke(); // no need to crash if empty
    }

    if (pressed_key.key == ESC) {
        end = true;
    }
    
    // TODO
    return 0;
}

int dispatch_timer_tick() {
    if (draw_frame() != OK) {
        printf("error while drawing frame\n");
        return 1;
    }

    return 0;
}

int draw_frame() {
    if (canvas_draw_frame(0) != OK)
       return 1;
    if (cursor_draw(CURSOR_PAINT) != OK)
       return 1;
    if (vg_flip_page() != OK)
        return 1;
    return 0;
}

bool should_end() {
    return end;
}
