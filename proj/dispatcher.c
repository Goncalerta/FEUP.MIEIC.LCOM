#include <lcom/lcf.h>
#include <stdarg.h>
#include "dispatcher.h"
#include "kbc.h"
#include "keyboard.h"
#include "mouse.h"
#include "video_gr.h"
#include "rtc.h"
#include "canvas.h"
#include "cursor.h"
#include "font.h"
#include "game.h"
#include "textbox.h"
#include "button.h"
#include "menu.h"
#include "protocol.h"

static bool end = false;
static bool bound_canvas = false;
static size_t num_listening_buttons = 0;
static button_t **listening_buttons = NULL;
static size_t num_listening_text_boxes = 0;
static text_box_t **listening_text_boxes = NULL;

// TODO not sure where this makes more sense
typedef enum player_state_t {
    NOT_READY,
    READY,
    RANDOM_NUMBER_SENT
} player_state_t;

player_state_t this_player_state = NOT_READY;
int this_player_random_number;
player_state_t other_player_state = NOT_READY;
int other_player_random_number;

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

int dispatcher_bind_text_boxes(size_t number_of_text_boxes, ...) {
    if (listening_text_boxes != NULL)
        free(listening_text_boxes);

    num_listening_text_boxes = number_of_text_boxes;
    listening_text_boxes = malloc(number_of_text_boxes * sizeof(text_box_t*));
    
    va_list ap;
    va_start(ap, number_of_text_boxes);
    for (size_t i = 0; i < number_of_text_boxes; i++) {
        listening_text_boxes[i] = va_arg(ap, text_box_t*);
    }
    va_end(ap);

    return 0;
}

int dispatcher_bind_canvas(bool is_to_bind) {
    bound_canvas = is_to_bind;
    if (!is_to_bind) {
        if (canvas_update_state(false, false, false) != OK)
            return 1;
    }

    return 0;
}

int event_start_round() {
    bool canvas_enabled = game_get_role() == DRAWER;
    if (canvas_init(vg_get_hres(), vg_get_vres() - GAME_BAR_HEIGHT, canvas_enabled) != OK)
        return 1;

    if (game_start_round() != OK)
        return 1;
    
    return 0;
}

int event_new_round_as_guesser(const char *word) {
    if (game_new_round(GUESSER, word) != OK)
        return 1;

    if (menu_set_new_round_screen(GUESSER) != OK)
        return 1;

    return 0;
}

int event_new_round_as_drawer() {
    const char *word;
    get_random_word(&word);

    if (game_new_round(DRAWER, word) != OK)
        return 1;

    if (protocol_send_new_round(word) != OK)
        return 1;

    if (menu_set_new_round_screen(DRAWER) != OK)
        return 1;

    static const rtc_alarm_time_t time_to_start_round = {.hours = 0, .minutes = 0, .seconds = 3};
    if (rtc_set_alarm_in(time_to_start_round) != OK)
        return 1;

    return 0;
}

int event_end_program() {
    end = true;
    return 0;
}

int event_round_win(uint32_t score) {
    if (game_round_over(score, true) != OK)
        return 1;
    if (protocol_send_round_win(score) != OK)
        return 1;
    return 0;
}

int event_end_round() {
    if (canvas_exit() != OK)
        return 1;
    game_delete_round();
    return 0;
}

int event_new_stroke(bool primary_button) {
    if (canvas_new_stroke(drawer_get_selected_color(), drawer_get_selected_thickness()) != OK)
        return 1;
    if (protocol_send_new_stroke(drawer_get_selected_color(), drawer_get_selected_thickness()) != OK)
        return 1;
    
    return 0;
}

int event_new_atom(uint16_t x, uint16_t y) {
    if (canvas_new_stroke_atom(x, y) != OK)
        return 1;
    if (protocol_send_new_atom(x, y) != OK)
        return 1;

    return 0;
}

int event_undo() {
    if (canvas_undo_stroke() != OK)
        return 1;
    if (protocol_send_undo_canvas() != OK)
        return 1;

    return 0;
}

int event_redo() {
    if (canvas_redo_stroke() != OK)
        return 1;
    if (protocol_send_redo_canvas() != OK)
        return 1;

    return 0;
}

int event_ready_to_play() {
    if (menu_set_awaiting_player_menu() != OK)
        return 1;
    if (protocol_send_ready_to_play() != OK)
        return 1;
    this_player_state = READY;
    if (other_player_state == READY) {
        if (event_this_player_random_number() != OK)
            return 1;
    }
    
    return 0;
}

int event_this_player_random_number() {
    if (other_player_state == NOT_READY) {
        return 0;
    }

    this_player_state = RANDOM_NUMBER_SENT;
    this_player_random_number = rand();
    if (protocol_send_random_number(this_player_random_number) != OK)
        return 1;

    if (other_player_state == RANDOM_NUMBER_SENT) {
        if (this_player_random_number > other_player_random_number) {
            // Player is about to begin game. Should not be ready to start a new one while this one is ongoing
            this_player_state = NOT_READY;
            other_player_state = NOT_READY;
            if (new_game() != OK)
                return 1;
            if (event_new_round_as_drawer() != OK)
                return 1;
        } else if (this_player_random_number < other_player_random_number) {
            // Player is about to begin game. Should not be ready to start a new one while this one is ongoing
            this_player_state = NOT_READY;
            other_player_state = NOT_READY;
            if (new_game() != OK)
                return 1;
        } else {
            this_player_state = READY;
            other_player_state = READY;
            if (event_this_player_random_number() != OK)
                return 1;
        }
    }

    return 0;
}

int event_other_player_opened_program() {
    if (this_player_state == READY) {
        if (protocol_send_ready_to_play() != OK)
            return 1;
    }

    return 0;
}

int event_other_player_random_number(int random_number) {
    if (this_player_state == NOT_READY) {
        return 0;
    }

    other_player_state = RANDOM_NUMBER_SENT;
    other_player_random_number = random_number;

    if (this_player_state == RANDOM_NUMBER_SENT) {
        if (this_player_random_number > other_player_random_number) {
            // Player is about to begin game. Should not be ready to start a new one while this one is ongoing
            this_player_state = NOT_READY;
            other_player_state = NOT_READY;
            if (new_game() != OK)
                return 1;
            if (event_new_round_as_drawer() != OK)
                return 1;
        } else if (this_player_random_number < other_player_random_number) {
            // Player is about to begin game. Should not be ready to start a new one while this one is ongoing
            this_player_state = NOT_READY;
            other_player_state = NOT_READY;
            if (new_game() != OK)
                return 1;
        } else {
            this_player_state = READY;
            other_player_state = READY;
            if (event_this_player_random_number() != OK)
                return 1;
        }
    }

    return 0;
}

int event_other_player_ready_to_play() {
    other_player_state = READY;
    if (this_player_state == READY || this_player_state == RANDOM_NUMBER_SENT) {
        if (event_this_player_random_number() != OK)
            return 1;
    }

    return 0;
}

int event_other_player_leave_game() {
    other_player_state = NOT_READY;
    if (menu_is_game_ongoing()) {
        if (rtc_disable_int(ALARM_INTERRUPT) != OK)
            return 1;
        if (event_end_round() != OK)
            return 1;
        if (menu_set_other_player_left_screen() != OK)
            return 1;
    }

    if (this_player_state == RANDOM_NUMBER_SENT) {
        this_player_state = READY;
    }

    return 0;
}

int event_leave_game() {
    if (rtc_disable_int(ALARM_INTERRUPT) != OK)
        return 1;
    delete_game();
    if (canvas_exit() != OK)
        return 1;
    if (protocol_send_leave_game() != OK)
        return 1;
    if (menu_set_main_menu() != OK)
        return 1;
    this_player_state = NOT_READY;
    if (other_player_state == RANDOM_NUMBER_SENT) {
        other_player_state = READY;
    }
    
    return 0;
}

int dispatch_mouse_packet(struct packet p) {
    if (p.x_ov || p.y_ov)
        return 1;
    cursor_move(p.delta_x, p.delta_y);
    cursor_update_buttons(p.lb, p.rb);
    if (event_update_cursor_state() != OK)
        return 1;
    return 0;
}

int event_update_cursor_state() {
    int16_t x = cursor_get_x();
    int16_t y = cursor_get_y();
    bool lb = cursor_get_lb();
    bool rb = cursor_get_rb();

    bool hovering = false;
    cursor_set_state(CURSOR_ARROW);

    for (size_t i = 0; i < num_listening_buttons; i++) {
        button_t *button = listening_buttons[i];
        if (!hovering && button_is_hovering(*button, x, y)) {
            hovering = true;
            if (button_update_state(button, true, lb, rb) != OK)
                return 1;
        } else {
            if (button_update_state(button, false, lb, rb) != OK)
                return 1;
        }
    }
    
    for (size_t i = 0; i < num_listening_text_boxes; i++) {
        text_box_t *text_box = listening_text_boxes[i];
        if (!hovering && text_box_is_hovering(*text_box, x, y)) {
            hovering = true;
            if (text_box_update_state(text_box, true, lb, rb, x, y) != OK)
                return 1;
        } else {
            if (text_box_update_state(text_box, false, lb, rb, x, y) != OK)
                return 1;
        }
        if (text_box->state != TEXT_BOX_NORMAL && text_box->state != TEXT_BOX_SELECTED_NOT_HOVERING) {
            cursor_set_state(CURSOR_WRITE);
        }
    }

    if (bound_canvas) {
        if (!hovering && canvas_is_hovering(x, y)) {
            hovering = true;
            if (canvas_update_state(true, lb, rb) != OK)
                return 1;
        } else {
            if (canvas_update_state(false, lb, rb) != OK)
                return 1;
        }
        
        if (canvas_get_state() != CANVAS_STATE_NORMAL) {
            if (canvas_is_enabled()) {
                cursor_set_state(CURSOR_PAINT);
            } else {
                cursor_set_state(CURSOR_DISABLED);
            }
        } 
    }

    return 0;
}

int event_guess_word(char *guess) {
    if (guess != NULL && strncmp(guess, "", 1)) {
        if (game_is_round_ongoing()) {
            if (game_guess_word(guess) != OK) 
                return 1;
            if (protocol_send_guess(guess) != OK)
                return 1;
        }
    } else {
        free(guess);
    }

    return 0;
}

int dispatch_keyboard_event(kbd_event_t kbd_event) {
    if (kbd_event.key == ESC && !kbd_event.is_ctrl_pressed) {
        if (menu_get_state() == PAUSE_MENU) {
            if (game_resume() != OK)
                return 1;
            return 0;
        } else if (menu_get_state() == GAME) {
            if (menu_set_pause_menu() != OK) 
                return 1;
            return 0;
        }
    }
    
    for (size_t i = 0; i < num_listening_text_boxes; i++) {
        text_box_t *text_box = listening_text_boxes[i];
        if (text_box_react_kbd(text_box, kbd_event) != OK) {
            return 1;
        }
    }

    if (bound_canvas) {
        if (canvas_react_kbd(kbd_event) != OK)
            return 1;
    }

    return 0;
}

int dispatch_timer_tick() {
    if (menu_get_state() == GAME || menu_get_state() == PAUSE_MENU) {
        if (game_timer_tick() != OK)
            return 1;
    }

    if (draw_frame() != OK) {
        printf("error while drawing frame\n");
        return 1;
    }

    return 0;
}

int dispatch_rtc_alarm_int() {
    if (menu_get_state() == GAME || menu_get_state() == PAUSE_MENU) {
        if (game_rtc_alarm() != OK)
            return 1;
    } else if (menu_get_state() == DRAWER_NEW_ROUND_SCREEN) {
        if (rtc_disable_int(ALARM_INTERRUPT) != OK)
            return 1;
        if (protocol_send_start_round() != OK)
            return 1;
        if (event_start_round() != OK)
            return 1;
    }
    
    return 0;
}

int dispatch_rtc_periodic_int() {
    if (menu_get_state() == GAME || menu_get_state() == PAUSE_MENU) {
        if (game_rtc_pi_tick() != OK)
            return 1;
    }
    return 0;
}

int draw_frame() {
    menu_state_t state = menu_get_state();

    if (state == GAME || state == PAUSE_MENU) {
        if (canvas_draw_frame(0) != OK)
            return 1;
        if (game_draw() != OK)
            return 1;
    }

    if (state != GAME) {
        if (menu_draw() != OK)
            return 1;
    }

    if (date_draw_current() != OK)
        return 1;
    if (cursor_draw() != OK)
        return 1;
    if (vg_flip_page() != OK)
        return 1;

    return 0;
}

bool should_end() {
    return end;
}
