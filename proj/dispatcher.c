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
#include "queue.h"

#define EVENTS_TO_HANDLE_CAPACITY 16 // starting capacity of events_to_handle queue

static bool end = false;

static queue_t events_to_handle;
static bool bound_canvas = false;
static size_t num_listening_buttons = 0;
static button_t **listening_buttons = NULL;
static size_t num_listening_text_boxes = 0;
static text_box_t **listening_text_boxes = NULL;

typedef enum player_state_t {
    NOT_READY,
    READY,
    RANDOM_NUMBER_SENT
} player_state_t;

static player_state_t this_player_state = NOT_READY;
static int this_player_random_number = 0;
static player_state_t other_player_state = NOT_READY;
static int other_player_random_number = 0;

int dispatcher_reset_bindings() {
    if (dispatcher_bind_buttons(0) != OK)
        return 1;
    if (dispatcher_bind_text_boxes(0) != OK)
        return 1;
    if (dispatcher_bind_canvas(false) != OK)
        return 1;
    return 0;
}

int dispatcher_bind_buttons(size_t number_of_buttons, ...) {
    if (listening_buttons != NULL)
        free(listening_buttons);

    num_listening_buttons = number_of_buttons;
    if (number_of_buttons == 0) {
        listening_buttons = NULL;
    } else {
        listening_buttons = malloc(number_of_buttons * sizeof(button_t*));
        if (listening_buttons == NULL)
            return 1;
    }
    
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
    if (number_of_text_boxes == 0) {
        listening_text_boxes = NULL;
    } else {
        listening_text_boxes = malloc(number_of_text_boxes * sizeof(text_box_t*));
        if (listening_text_boxes == NULL)
            return 1;
    }
    
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

int dispatcher_init() {
    if (new_queue(&events_to_handle, sizeof(event_t), EVENTS_TO_HANDLE_CAPACITY) != OK)
        return 1;
    return 0;
}

void dispatcher_exit() {
    dispatcher_reset_bindings();
    
    // Queue must be manually emptied so that messages can be properly freed
    while (!queue_is_empty(&events_to_handle)) {
        event_t event;
        if (queue_top(&events_to_handle, &event) != OK)
            return;
        if (queue_pop(&events_to_handle) != OK)
            return;
        if (event.type == UART_MESSAGE_EVENT) {
            protocol_delete_message(&event.content.uart_message);
        }
    }

    delete_queue(&events_to_handle);
}

int dispatcher_queue_mouse_event() {
    event_content_t content;

    if (mouse_retrieve_packet(&content.mouse_packet) != OK)
        return 1;

    event_t event;
    event.type = MOUSE_EVENT;
    event.content = content;

    if (queue_push(&events_to_handle, &event) != OK)
        return 1;
    
    return 0;
}

int dispatcher_queue_keyboard_event() {
    event_content_t content;

    if (kbd_handle_scancode(&content.kbd_state) != OK)
        return 1;

    event_t event;
    event.type = KEYBOARD_EVENT;
    event.content = content;

    if (queue_push(&events_to_handle, &event) != OK)
        return 1;
    
    return 0;
}

int dispatcher_queue_rtc_periodic_interrupt_event() {
    event_content_t content;
    content.no_content = NULL;

    event_t event;
    event.type = RTC_PERIODIC_INTERRUPT_EVENT;
    event.content = content;

    if (queue_push(&events_to_handle, &event) != OK)
        return 1;
    return 0;
}

int dispatcher_queue_rtc_alarm_event() {
    event_content_t content;
    content.no_content = NULL;
    
    event_t event;
    event.type = RTC_ALARM_EVENT;
    event.content = content;

    if (queue_push(&events_to_handle, &event) != OK)
        return 1;
    return 0;
}

int dispatcher_queue_uart_message_event(message_t message) {
    event_content_t content;
    content.uart_message = message;

    event_t event;
    event.type = UART_MESSAGE_EVENT;
    event.content = content;

    if (queue_push(&events_to_handle, &event) != OK)
        return 1;
    return 0;
}

int dispatcher_queue_timer_tick_event() {
    event_content_t content;
    content.no_content = NULL;
    
    event_t event;
    event.type = TIMER_TICK_EVENT;
    event.content = content;

    if (queue_push(&events_to_handle, &event) != OK)
        return 1;
    return 0;
}

int dispatcher_dispatch_events() {
    while (!queue_is_empty(&events_to_handle)) {
        event_t event;
        if (queue_top(&events_to_handle, &event) != OK)
            return 1;
        if (queue_pop(&events_to_handle) != OK)
            return 1;
        
        switch (event.type) {
            case MOUSE_EVENT:
                if (dispatch_mouse_packet(event.content.mouse_packet) != OK)
                    return 1;
                break;
            case KEYBOARD_EVENT:
                if (dispatch_keyboard_event(event.content.kbd_state) != OK)
                    return 1;
                break;
            case RTC_PERIODIC_INTERRUPT_EVENT:
                if (dispatch_rtc_periodic_int() != OK)
                    return 1;
                break;
            case RTC_ALARM_EVENT:
                if (dispatch_rtc_alarm_int() != OK)
                    return 1;
                break;
            case UART_MESSAGE_EVENT:
                if (dispatch_uart_message(event.content.uart_message) != OK)
                    return 1;
                break;
            case TIMER_TICK_EVENT:
                if (dispatch_timer_tick() != OK)
                    return 1;
                break;
            default:
                return 1;
        }
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

int dispatch_rtc_periodic_int() {
    if (menu_get_state() == GAME || menu_get_state() == PAUSE_MENU) {
        if (game_rtc_pi_tick() != OK)
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

int dispatch_uart_message(message_t message) {
    // Protocol handler will take care of deleting the message
    if (protocol_handle_message_event(&message) != OK)
        return 1;
    return 0;
}

int dispatch_timer_tick() {
    if (menu_get_state() == GAME || menu_get_state() == PAUSE_MENU) {
        if (game_timer_tick() != OK)
            return 1;
    }

    

    return 0;
}

int draw_frame() {
    menu_state_t state = menu_get_state();

    if (state == GAME || state == PAUSE_MENU) {
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

int event_other_player_opened_program() {
    if (this_player_state == READY) {
        if (protocol_send_ready_to_play() != OK)
            return 1;
    }

    return 0;
}

int event_notify_not_in_game() {
    if (protocol_send_leave_game() != OK)
        return 1;
    if (this_player_state == RANDOM_NUMBER_SENT) {
        this_player_state = READY;
    }
    if (this_player_state == READY) {
        if (protocol_send_ready_to_play() != OK)
            return 1;
    }
    return 0;
}

int event_leave_game() {
    if (rtc_disable_int(ALARM_INTERRUPT) != OK)
        return 1;
    delete_game();
    canvas_exit();
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

int event_other_player_leave_game() {
    other_player_state = NOT_READY;
    if (menu_is_game_ongoing() && !game_is_over()) {
        if (rtc_disable_int(ALARM_INTERRUPT) != OK)
            return 1;
        if (event_end_round() != OK)
            return 1;
        game_set_over();
        if (menu_set_other_player_left_screen() != OK)
            return 1;
    }

    if (this_player_state == RANDOM_NUMBER_SENT) {
        this_player_state = READY;
    }

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

int event_other_player_ready_to_play() {
    other_player_state = READY;
    if (this_player_state == READY || this_player_state == RANDOM_NUMBER_SENT) {
        if (event_this_player_random_number() != OK)
            return 1;
    }

    return 0;
}

static int compare_random_numbers() {
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
        if (compare_random_numbers() != OK)
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
        if (compare_random_numbers() != OK)
            return 1;
    }

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
    const char *word = get_random_word();

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

int event_start_round() {
    bool canvas_enabled = game_get_role() == DRAWER;
    if (canvas_init(vg_get_hres(), vg_get_vres() - GAME_BAR_HEIGHT, canvas_enabled) != OK)
        return 1;

    if (game_start_round() != OK)
        return 1;
    
    return 0;
}

int event_end_round() {
    canvas_exit();
    game_delete_round();
    return 0;
}

int event_new_stroke() {
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

int event_guess_word(char *guess) {
    if (guess != NULL && strncmp(guess, "", 1) && game_is_round_ongoing()) {
        if (game_guess_word(guess) != OK) {
            free(guess);
            return 1;
        }
            
        if (protocol_send_guess(guess) != OK) {
            free(guess);
            return 1;
        }       
    } else {
        free(guess);
    }

    return 0;
}

int event_round_win(uint32_t score) {
    if (game_round_over(score, true) != OK)
        return 1;
    if (protocol_send_round_win(score) != OK)
        return 1;
    return 0;
}

int event_end_program() {
    end = true;
    return 0;
}

bool should_end() {
    return end;
}
