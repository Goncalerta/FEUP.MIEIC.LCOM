#include <lcom/lcf.h>

#include "menu.h"
#include "button.h"
#include "dispatcher.h"
#include "video_gr.h"
#include "graphics.h"
#include "game.h"
#include "cursor.h"
#include "date.h"

#include "xpm/menu_new_game.xpm"
#include "xpm/menu_exit_game.xpm"
#include "xpm/menu_resume.xpm"
#include "xpm/menu_main_menu.xpm"

#define MENU_BUTTON_WIDTH 320
#define MENU_BUTTON_HEIGHT 100
#define MENU_BUTTON_DISTANCE 50
#define MENU_BACKGROUND_COLOR 0x8c2d19
#define MENU_GREETING_X 10
#define MENU_GREETING_Y 740

static menu_state_t menu_state;
//static xpm_image_t back_ground; TODO add a background image to main menu?

// MAIN MENU:
static button_t b_new_game;
static button_t b_end_program;

// PAUSE_MENU:
static button_t b_resume;
static button_t b_back_to_main_menu;

// AWAITING PLAYER:
static uint8_t awaiting_player_tick;


int menu_init(enum xpm_image_type type) {
    frame_buffer_t buf = vg_get_back_buffer();
    uint16_t x = (buf.h_res - MENU_BUTTON_WIDTH)/2;
    uint16_t y = buf.v_res/2 - MENU_BUTTON_DISTANCE/2 - MENU_BUTTON_HEIGHT;

    xpm_image_t new_game;
    if (xpm_load(xpm_menu_new_game, type, &new_game) == NULL)
        return 1;
    new_button(&b_new_game, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, event_ready_to_play);
    button_set_xpm_icon(&b_new_game, new_game);
    
    xpm_image_t resume;
    if (xpm_load(xpm_menu_resume, type, &resume) == NULL)
        return 1;
    new_button(&b_resume, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, game_resume);
    button_set_xpm_icon(&b_resume, resume);

    y += MENU_BUTTON_HEIGHT + MENU_BUTTON_DISTANCE;
    xpm_image_t exit;
    if (xpm_load(xpm_menu_exit_game, type, &exit) == NULL)
        return 1;
    new_button(&b_end_program, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, event_end_program);
    button_set_xpm_icon(&b_end_program, exit);

    xpm_image_t main_menu;
    if (xpm_load(xpm_menu_main_menu, type, &main_menu) == NULL)
        return 1;
    new_button(&b_back_to_main_menu, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, event_leave_game);
    button_set_xpm_icon(&b_back_to_main_menu, main_menu);

    menu_state = MAIN_MENU;
    return 0;
}

int menu_draw() {
    frame_buffer_t buf = vg_get_back_buffer();

    switch (menu_state) {
    case MAIN_MENU:
        if (vb_fill_screen(buf, MENU_BACKGROUND_COLOR) != OK)
            return 1;
        if (button_draw(buf, b_new_game) != OK)
            return 1;
        if (button_draw(buf, b_end_program) != OK)
            return 1;
        if (date_draw_greeting(MENU_GREETING_X, MENU_GREETING_Y) != OK)
            return 1;
        break;

    case PAUSE_MENU:
        if (button_draw(buf, b_resume) != OK)
            return 1;
        if (button_draw(buf, b_back_to_main_menu) != OK)
            return 1;
        break;
    
    case WORD_SCREEN:
        if (vb_fill_screen(buf, MENU_BACKGROUND_COLOR) != OK)
            return 1;
        if (draw_game_correct_guess() != OK)
            return 1;
        break;

    case AWAITING_OTHER_PLAYER:
        if (vb_fill_screen(buf, MENU_BACKGROUND_COLOR) != OK)
            return 1;

        size_t str_size;
        if (awaiting_player_tick < 60) {
            str_size = 25;
        } else if (awaiting_player_tick < 120) {
            str_size = 26;
        } else if (awaiting_player_tick < 180) {
            str_size = 27;
        } else {
            str_size = 25;
            awaiting_player_tick = 0;
        }
        awaiting_player_tick++;
        if (font_draw_string_centered(buf, "WAITING FOR OTHER PLAYER...", vg_get_hres()/2, vg_get_vres()/2 - 50, 0, str_size) != OK)
            return 1;
        if (button_draw(buf, b_back_to_main_menu) != OK)
            return 1;
        break;

    default:
        break;
    }

    return 0;
}

menu_state_t menu_get_state() {
    return menu_state;
}

void menu_set_state(menu_state_t state) {
    menu_state = state;
}

int menu_set_main_menu() {
    // TODO a dispatcher function called "reset bindings" that does most of this
    if (dispatcher_bind_buttons(2, &b_new_game, &b_end_program) != OK)
        return 1;
    if (dispatcher_bind_text_boxes(0) != OK)
        return 1;
    dispatcher_bind_canvas(false);

    cursor_set_state(CURSOR_ARROW);
    menu_state = MAIN_MENU;
    return 0;
}

int menu_set_pause_menu() {
    if (dispatcher_bind_buttons(2, &b_resume, &b_back_to_main_menu) != OK)
        return 1;
    if (dispatcher_bind_text_boxes(0) != OK)
        return 1;
    dispatcher_bind_canvas(false);

    cursor_set_state(CURSOR_ARROW);
    menu_state = PAUSE_MENU;
    return 0;
}

int menu_set_awaiting_player_menu() {
    if (dispatcher_bind_buttons(1, &b_back_to_main_menu) != OK)
        return 1;
    if (dispatcher_bind_text_boxes(0) != OK)
        return 1;
    dispatcher_bind_canvas(false);

    cursor_set_state(CURSOR_ARROW);
    menu_state = AWAITING_OTHER_PLAYER;
    awaiting_player_tick = 0;
    return 0;
}
