#include <lcom/lcf.h>
#include "dispatcher.h"
#include "kbc.h"
#include "keyboard.h"
#include "mouse.h"
#include "video_gr.h"
#include "canvas.h"
#include "cursor.h"
#include "font.h"
#include "game.h"

static bool end = false;

int event_new_game() {
    canvas_init(vg_get_hres(), vg_get_vres() - GAME_BAR_HEIGHT);
    game_start_round();
    return 0;
}

int event_end_round() {
    clear_canvas();
    game_start_round();
    return 0;
}

int event_new_stroke(bool primary_button) {
    if (primary_button) {
        if (canvas_new_stroke(0x000033ff, 10) != OK)
            return 1;
    } else {
        if (canvas_new_stroke(0x00FFFFFF, 10) != OK)
            return 1;
    }
    
    return 0;
}

int event_new_atom(uint16_t x, uint16_t y) {
    if (canvas_new_stroke_atom(x, y) != OK)
        return 1;
    return 0;
}

int dispatch_mouse_packet(struct packet p) {
    cursor_move(p.delta_x, p.delta_y);
    bool hovering = false;

    if (!hovering && canvas_is_hovering(cursor_get_x(), cursor_get_y())) {
        hovering = true;
    }
    if (canvas_update_state(hovering, p.lb, p.rb) != OK)
        return 1;
    
    if (canvas_get_state() != CANVAS_STATE_NORMAL)
        cursor_set_state(CURSOR_PAINT);
    else 
        cursor_set_state(CURSOR_ARROW);

    return 0;
}

int dispatch_keyboard_event(kbd_state pressed_key) {
    // just to check if it's correct
    if (pressed_key.key == CHAR && !kbd_is_ctrl_pressed()) {
        if (font_draw_char(vg_get_back_buffer(), pressed_key.char_key, 10, 10) != 0) {
            printf("font_draw_char failed\n");
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
    game_round_timer_tick();
    if (draw_frame() != OK) {
        printf("error while drawing frame\n");
        return 1;
    }

    return 0;
}

int draw_frame() {
    if (canvas_draw_frame(0) != OK)
        return 1;
    if (draw_game_bar() != OK)
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
