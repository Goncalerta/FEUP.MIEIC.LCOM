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

static menu_state_t menu_state = MAIN_MENU;

// MAIN MENU:
static button_t b_new_game;
static button_t b_end_program;

// PAUSE_MENU:
static button_t b_resume;
static button_t b_back_to_main_menu;

// AWAITING PLAYER:
static uint8_t awaiting_player_tick = 0;

static xpm_image_t xpm_new_game, xpm_resume, xpm_exit, xpm_main_menu;

int menu_init(enum xpm_image_type type) {
    frame_buffer_t buf = vg_get_back_buffer();
    uint16_t x = (buf.h_res - MENU_BUTTON_WIDTH)/2;
    uint16_t y = buf.v_res/2 - MENU_BUTTON_DISTANCE/2 - MENU_BUTTON_HEIGHT;

    if (xpm_load(xpm_menu_new_game, type, &xpm_new_game) == NULL)
        return 1;
    new_button(&b_new_game, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, event_ready_to_play);
    button_set_xpm_icon(&b_new_game, xpm_new_game);
    
    if (xpm_load(xpm_menu_resume, type, &xpm_resume) == NULL)
        return 1;
    new_button(&b_resume, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, game_resume);
    button_set_xpm_icon(&b_resume, xpm_resume);

    y += MENU_BUTTON_HEIGHT + MENU_BUTTON_DISTANCE;

    if (xpm_load(xpm_menu_exit_game, type, &xpm_exit) == NULL)
        return 1;
    new_button(&b_end_program, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, event_end_program);
    button_set_xpm_icon(&b_end_program, xpm_exit);

    if (xpm_load(xpm_menu_main_menu, type, &xpm_main_menu) == NULL)
        return 1;
    new_button(&b_back_to_main_menu, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, event_leave_game);
    button_set_xpm_icon(&b_back_to_main_menu, xpm_main_menu);

    menu_state = MAIN_MENU;
    return 0;
}

void menu_exit() {
    free(xpm_new_game.bytes);
    free(xpm_resume.bytes);
    free(xpm_exit.bytes);
    free(xpm_main_menu.bytes);
}

bool menu_is_game_ongoing() {
    return menu_state == DRAWER_NEW_ROUND_SCREEN || menu_state == GUESSER_NEW_ROUND_SCREEN 
        || menu_state == GAME || menu_state == PAUSE_MENU;
}

static int menu_draw_game_over_screen(const char *reason, size_t reason_size) {
    frame_buffer_t buf = vg_get_back_buffer();
    
    if (vb_fill_screen(buf, MENU_BACKGROUND_COLOR) != OK)
        return 1;

    if (font_draw_string_centered(buf, "GAME OVER", vg_get_hres()/2, 250, 0, 9) != OK)
        return 1;
    
    if (font_draw_string_centered(buf, reason, vg_get_hres()/2, 285, 0, reason_size) != OK)
        return 1;
    if (font_draw_string(buf, "ROUND", vg_get_hres()/2 - 140, 330) != OK)
        return 1;
    if (font_draw_string(buf, "SCORE", vg_get_hres()/2 - 140, 350) != OK)
        return 1;

    char round_display[6];
    sprintf(round_display, "%5d", game_get_round_number());
    if (font_draw_string(buf, round_display, vg_get_hres()/2 + 40, 330) != OK)
        return 1;

    char score_display[6];
    sprintf(score_display, "%05d", game_get_score());
    if (font_draw_string(buf, score_display, vg_get_hres()/2 + 40, 350) != OK)
        return 1;

    if (button_draw(buf, b_back_to_main_menu) != OK)
        return 1;

    return 0;
}

static int menu_draw_new_round_screen(role_t role) {
    frame_buffer_t buf = vg_get_back_buffer();
    
    if (vb_fill_screen(buf, MENU_BACKGROUND_COLOR) != OK)
        return 1;

    if (font_draw_string(buf, "ROUND", vg_get_hres()/2 - 140, 330) != OK)
        return 1;
    if (font_draw_string(buf, "SCORE", vg_get_hres()/2 - 140, 350) != OK)
        return 1;

    char round_display[6];
    sprintf(round_display, "%5d", game_get_round_number());
    if (font_draw_string(buf, round_display, vg_get_hres()/2 + 40, 330) != OK)
        return 1;

    char score_display[6];
    sprintf(score_display, "%05d", game_get_score());
    if (font_draw_string(buf, score_display, vg_get_hres()/2 + 40, 350) != OK)
        return 1;    

    switch (role) {
    case DRAWER:
        if (font_draw_string_centered(buf, "YOU ARE THE DRAWER", vg_get_hres()/2, 400, 0, 18) != OK)
            return 1;
        if (font_draw_string_centered(buf, "TRY TO DRAW THE FOLLOWING WORD", vg_get_hres()/2, 430, 0, 30) != OK)
            return 1;

        const char *word = game_get_correct_word();

        if (font_draw_string_centered(buf, word, vg_get_hres()/2, 480, 0, strlen(word)) != OK)
            return 1;
        break;
    case GUESSER:
        if (font_draw_string_centered(buf, "YOU ARE THE GUESSER", vg_get_hres()/2, 400, 0, 19) != OK)
            return 1;
        if (font_draw_string_centered(buf, "TRY TO GUESS THE WORD BEING DRAWN", vg_get_hres()/2, 430, 0, 33) != OK)
            return 1;
        break;
    default:
        break;
    }
    
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
    
    case DRAWER_NEW_ROUND_SCREEN:
        if (menu_draw_new_round_screen(DRAWER) != OK)
            return 1;
        break;

    case GUESSER_NEW_ROUND_SCREEN:
        if (menu_draw_new_round_screen(GUESSER) != OK)
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
    case GAME_OVER_SCREEN:
        if (menu_draw_game_over_screen("YOU RUN OUT OF TIME", 19) != OK)
            return 1;
        break;
    case OTHER_PLAYER_LEFT_SCREEN:
        if (menu_draw_game_over_screen("OTHER PLAYER HAS LEFT THE GAME", 30) != OK)
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
    if (dispatcher_reset_bindings() != OK)
        return 1;
    if (dispatcher_bind_buttons(2, &b_new_game, &b_end_program) != OK)
        return 1;

    if (event_update_cursor_state() != OK)
        return 1;

    menu_state = MAIN_MENU;
    return 0;
}

int menu_set_pause_menu() {
    if (dispatcher_reset_bindings() != OK)
        return 1;
    if (dispatcher_bind_buttons(2, &b_resume, &b_back_to_main_menu) != OK)
        return 1;

    if (event_update_cursor_state() != OK)
        return 1;
    
    menu_state = PAUSE_MENU;
    return 0;
}

int menu_set_awaiting_player_menu() {
    if (dispatcher_reset_bindings() != OK)
        return 1;
    if (dispatcher_bind_buttons(1, &b_back_to_main_menu) != OK)
        return 1;

    if (event_update_cursor_state() != OK)
        return 1;
    
    menu_state = AWAITING_OTHER_PLAYER;
    awaiting_player_tick = 0;
    return 0;
}

int menu_set_new_round_screen(role_t role) {
    if (dispatcher_reset_bindings() != OK)
        return 1;

    if (event_update_cursor_state() != OK)
        return 1;
    
    switch (role) {
    case DRAWER:
        menu_state = DRAWER_NEW_ROUND_SCREEN;
        break;
    case GUESSER:
        menu_state = GUESSER_NEW_ROUND_SCREEN;
        break;
    default:
        break;
    }
    
    return 0;
}

int menu_set_game_over_screen() {
    if (dispatcher_reset_bindings() != OK)
        return 1;
    if (dispatcher_bind_buttons(1, &b_back_to_main_menu) != OK)
        return 1;

    if (event_update_cursor_state() != OK)
        return 1;
    
    menu_state = GAME_OVER_SCREEN;
    return 0;
}

int menu_set_other_player_left_screen() {
    if (dispatcher_reset_bindings() != OK)
        return 1;
    if (dispatcher_bind_buttons(1, &b_back_to_main_menu) != OK)
        return 1;

    if (event_update_cursor_state() != OK)
        return 1;

    menu_state = OTHER_PLAYER_LEFT_SCREEN;
    return 0;
}
