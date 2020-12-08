#include <lcom/lcf.h>
#include <stdarg.h>
#include "dispatcher.h"
#include "kbc.h"
#include "keyboard.h"
#include "mouse.h"
#include "video_gr.h"
#include "canvas.h"
#include "cursor.h"
#include "font.h"
#include "game.h"
#include "textbox.h"
#include "button.h"

static bool end = false;
size_t num_listening_buttons = 0;
static button_t **listening_buttons = NULL;

int dispatcher_bind_buttons(size_t number_of_buttons, ...) {
    if (listening_buttons != NULL)
        free(listening_buttons);

    num_listening_buttons = number_of_buttons;
    listening_buttons = malloc(number_of_buttons * sizeof(button_t*));
    
    va_list ap;
    va_start(ap, number_of_buttons);
    for (size_t i = 0; i < number_of_buttons; i++) {
        listening_buttons[i] = va_arg(ap, button_t*);
    }
    va_end(ap);

    return 0;
}

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
        if (canvas_new_stroke(game_get_selected_color(), 10) != OK)
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

    for (size_t i = 0; i < num_listening_buttons; i++) {
        button_t *button = listening_buttons[i];
        if (!hovering && button_is_hovering(*button, cursor_get_x(), cursor_get_y())) {
            hovering = true;
            if (button_update_state(button, true, p.lb, p.rb) != OK)
                return 1;
        } else {
            if (button_update_state(button, false, p.lb, p.rb) != OK)
                return 1;
        }
    }

    if (!hovering && canvas_is_hovering(cursor_get_x(), cursor_get_y())) {
        hovering = true;
        if (canvas_update_state(true, p.lb, p.rb) != OK)
            return 1;
    } else {
        if (canvas_update_state(false, p.lb, p.rb) != OK)
            return 1;
    }
    
    
    if (canvas_get_state() != CANVAS_STATE_NORMAL)
        cursor_set_state(CURSOR_PAINT);
    else 
        cursor_set_state(CURSOR_ARROW);

    return 0;
}

int dispatch_keyboard_event(kbd_state pressed_key) {
    text_box_react(GUESSER, pressed_key);

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
