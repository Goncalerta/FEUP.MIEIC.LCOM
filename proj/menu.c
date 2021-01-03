#include <lcom/lcf.h>

#include "menu.h"
#include "button.h"
#include "dispatcher.h"
#include "video_gr.h"
#include "graphics.h"
#include "game.h"
#include "cursor.h"
#include "date.h"
#include "font.h"

#include "xpm/menu_new_game.xpm"
#include "xpm/menu_exit_game.xpm"
#include "xpm/menu_resume.xpm"
#include "xpm/menu_main_menu.xpm"

/** @defgroup menu menu
 * @{
 *
 */

#define MENU_BUTTON_WIDTH 320 /*!< @brief Width of menu buttons */
#define MENU_BUTTON_HEIGHT 100 /*!< @brief Height of menu buttons */
#define MENU_BUTTON_DISTANCE 50 /*!< @brief Distance between menu buttons */
#define MENU_BACKGROUND_COLOR 0x8c2d19 /*!< @brief Menu background color */
#define MENU_GREETING_Y 180 /*!< @brief Y coordinate of main menu greeting */

static menu_state_t menu_state = MAIN_MENU; /*!< @brief Current menu state */

static button_t *b_new_game; /*!< @brief Button to start a new game (main menu) */
static button_t *b_end_program; /*!< @brief Button close program (main menu) */

static button_t *b_resume; /*!< @brief Button to resume game (pause menu) */
static button_t *b_back_to_main_menu; /*!< @brief Button to return to main menu (awaiting player and pause menu) */

static uint8_t awaiting_player_tick = 0; /*!< @brief Ticks for animating "AWAITING OTHER PLAYER..." text */

static xpm_image_t xpm_new_game; /*!< @brief b_new_game icon XPM */
static xpm_image_t xpm_resume; /*!< @brief b_resume icon XPM */
static xpm_image_t xpm_exit; /*!< @brief b_end_program icon XPM */
static xpm_image_t xpm_main_menu; /*!< @brief b_back_to_main_menu icon XPM */

int menu_init(enum xpm_image_type type) {
    frame_buffer_t buf = vg_get_back_buffer();
    uint16_t x = (buf.h_res - MENU_BUTTON_WIDTH)/2;
    uint16_t y = buf.v_res/2 - MENU_BUTTON_DISTANCE/2 - MENU_BUTTON_HEIGHT;

    // Button icons
    bool fail = false
    if (xpm_load(xpm_menu_new_game, type, &xpm_new_game) == NULL) {
        xpm_menu_new_game.bytes = NULL;
        fail = true;
    }
    if (xpm_load(xpm_menu_resume, type, &xpm_resume) == NULL) {
        xpm_menu_resume.bytes = NULL;
        fail = true;
    }
    if (xpm_load(xpm_menu_exit_game, type, &xpm_exit) == NULL) {
        xpm_tick.bytes = NULL;
        fail = true;
    }
    if (xpm_load(xpm_menu_main_menu, type, &xpm_main_menu) == NULL) {
        xpm_tick.bytes = NULL;
        fail = true;
    }

    // Buttons
    b_new_game = new_button(x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, handle_ready_to_play);
    if (b_new_game == NULL) {
        fail = true;
    } else {
        button_set_xpm_icon(b_new_game, xpm_new_game);
    }

    b_resume = new_button(x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, game_resume);
    if (b_resume == NULL) {
        fail = true;
    } else {
        button_set_xpm_icon(b_resume, xpm_resume);
    }

    y += MENU_BUTTON_HEIGHT + MENU_BUTTON_DISTANCE;

    b_end_program = new_button(x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, trigger_end_program);
    if (b_end_program == NULL) {
        fail = true;
    } else {
        button_set_xpm_icon(b_end_program, xpm_exit);
    }
    
    b_back_to_main_menu = new_button(x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, handle_leave_game);
    if (b_back_to_main_menu == NULL) {
        fail = true;
    } else {
        button_set_xpm_icon(b_back_to_main_menu, xpm_main_menu);
    }

    if (fail) {
        menu_exit();
        return 1;
    }

    if (menu_set_main_menu() != OK) {
        menu_exit();
        return 1;
    }

    return 0;
}

void menu_exit() {
    delete_button(b_new_game);
    delete_button(b_end_program);
    delete_button(b_resume);
    delete_button(b_back_to_main_menu);
    free(xpm_new_game.bytes);
    free(xpm_resume.bytes);
    free(xpm_exit.bytes);
    free(xpm_main_menu.bytes);
}

int menu_react_kbd(kbd_event_t kbd_event) {
    if (kbd_event.key == ESC && !kbd_event.is_ctrl_pressed) {
        if (menu_get_state() == PAUSE_MENU) {
            if (game_resume() != OK)
                return 1;
        } else if (menu_get_state() == GAME) {
            if (menu_set_pause_menu() != OK)
                return 1;
        }
    }
    
    return 0;
}

bool menu_is_game_ongoing() {
    return menu_state == DRAWER_NEW_ROUND_SCREEN || menu_state == GUESSER_NEW_ROUND_SCREEN 
        || menu_state == GAME || menu_state == PAUSE_MENU;
}

/**
 * @brief Draws the main menu to the back buffer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int menu_draw_main_menu() {
    frame_buffer_t buf = vg_get_back_buffer();

    if (vb_fill_screen(buf, MENU_BACKGROUND_COLOR) != OK)
        return 1;
    if (button_draw(buf, b_new_game) != OK)
        return 1;
    if (button_draw(buf, b_end_program) != OK)
        return 1;
    if (date_draw_greeting(vg_get_hres()/2, MENU_GREETING_Y) != OK)
        return 1;
    
    return 0;
}

/**
 * @brief Draws the pause menu to the back buffer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int menu_draw_pause_menu() {
    frame_buffer_t buf = vg_get_back_buffer();

    if (button_draw(buf, b_resume) != OK)
        return 1;
    if (button_draw(buf, b_back_to_main_menu) != OK)
        return 1;
    return 0;
}

/**
 * @brief Draws the game over screen to the back buffer.
 * 
 * @param reason String with the reason why the game ended
 * @param reason_size size of reason string
 * @return Return 0 upon success and non-zero otherwise
 */
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

/**
 * @brief Draws the new round screen for the given role to the back buffer.
 * 
 * @param role role of the player in the new round
 * @return Return 0 upon success and non-zero otherwise
 */
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

/**
 * @brief Draws the awaiting other player menu to the back buffer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int menu_draw_awaiting_other_player_menu() {
    frame_buffer_t buf = vg_get_back_buffer();
    
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
    
    return 0;
}

int menu_draw() {
    switch (menu_state) {
    case MAIN_MENU:
        if (menu_draw_main_menu() != OK)
            return 1;
        break;
    case PAUSE_MENU:
        if (menu_draw_pause_menu() != OK)
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
        if (menu_draw_awaiting_other_player_menu() != OK)
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

void menu_set_game_screen() {
    menu_state = GAME;
}

int menu_set_main_menu() {
    if (dispatcher_reset_bindings() != OK)
        return 1;
    if (dispatcher_bind_buttons(2, b_new_game, b_end_program) != OK)
        return 1;

    if (handle_update_cursor_state() != OK)
        return 1;

    menu_state = MAIN_MENU;
    return 0;
}

int menu_set_pause_menu() {
    // So that selected objects become unselected
    dispatcher_unselect_buttons_textboxes_canvas();
    if (dispatcher_reset_bindings() != OK)
        return 1;
    if (dispatcher_bind_buttons(2, b_resume, b_back_to_main_menu) != OK)
        return 1;

    if (handle_update_cursor_state() != OK)
        return 1;
    
    menu_state = PAUSE_MENU;
    return 0;
}

int menu_set_awaiting_player_menu() {
    if (dispatcher_reset_bindings() != OK)
        return 1;
    if (dispatcher_bind_buttons(1, b_back_to_main_menu) != OK)
        return 1;

    if (handle_update_cursor_state() != OK)
        return 1;
    
    menu_state = AWAITING_OTHER_PLAYER;
    awaiting_player_tick = 0;
    return 0;
}

int menu_set_new_round_screen(role_t role) {
    if (dispatcher_reset_bindings() != OK)
        return 1;

    if (handle_update_cursor_state() != OK)
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
    if (dispatcher_bind_buttons(1, b_back_to_main_menu) != OK)
        return 1;

    if (handle_update_cursor_state() != OK)
        return 1;
    
    menu_state = GAME_OVER_SCREEN;
    return 0;
}

int menu_set_other_player_left_screen() {
    if (dispatcher_reset_bindings() != OK)
        return 1;
    if (dispatcher_bind_buttons(1, b_back_to_main_menu) != OK)
        return 1;

    if (handle_update_cursor_state() != OK)
        return 1;

    menu_state = OTHER_PLAYER_LEFT_SCREEN;
    return 0;
}

/**@}*/
