#include <lcom/lcf.h>

#include "menu.h"
#include "button.h"
#include "dispatcher.h"
#include "video_gr.h"
#include "graphics.h"
#include "game.h"
#include "cursor.h"

#include "xpm/menu_new_game.xpm"
#include "xpm/menu_exit_game.xpm"
#include "xpm/menu_resume.xpm"
#include "xpm/menu_main_menu.xpm"

#define MENU_BUTTON_WIDTH 320
#define MENU_BUTTON_HEIGHT 100
#define MENU_BUTTON_DISTANCE 50
#define MENU_BACKGROUND_COLOR 0x8c2d19

static menu_state_t menu_state;
//static xpm_image_t back_ground; TODO add a background image to main menu?

// MAIN MENU:
static button_t b_new_game;
static button_t b_end_program;

// PAUSE_MENU:
static button_t b_resume;
static button_t b_back_to_main_menu;


void menu_init(enum xpm_image_type type) {
    frame_buffer_t buf = vg_get_back_buffer();
    uint16_t x = (buf.h_res - MENU_BUTTON_WIDTH)/2;
    uint16_t y = buf.v_res/2 - MENU_BUTTON_DISTANCE/2 - MENU_BUTTON_HEIGHT;

    xpm_image_t new_game;
    xpm_load(xpm_menu_new_game, type, &new_game);
    new_button(&b_new_game, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, event_new_game);
    button_set_xpm_icon(&b_new_game, new_game);
    
    xpm_image_t resume;
    xpm_load(xpm_menu_resume, type, &resume);
    new_button(&b_resume, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, game_resume);
    button_set_xpm_icon(&b_resume, resume);

    y += MENU_BUTTON_HEIGHT + MENU_BUTTON_DISTANCE;
    xpm_image_t exit;
    xpm_load(xpm_menu_exit_game, type, &exit);
    new_button(&b_end_program, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, event_end_program);
    button_set_xpm_icon(&b_end_program, exit);

    xpm_image_t main_menu;
    xpm_load(xpm_menu_main_menu, type, &main_menu);
    new_button(&b_back_to_main_menu, x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, menu_set_main_menu);
    button_set_xpm_icon(&b_back_to_main_menu, main_menu);

    menu_state = MAIN_MENU;
}

int menu_draw() {
    frame_buffer_t buf = vg_get_back_buffer();

    switch (menu_state) {
    case MAIN_MENU:
        if (vb_fill_screen(buf, MENU_BACKGROUND_COLOR) != 0)
            return 1;
        if (button_draw(buf, b_new_game) != 0)
            return 1;
        if (button_draw(buf, b_end_program) != 0)
            return 1;
        break;

    case PAUSE_MENU:
        if (button_draw(buf, b_resume) != 0)
            return 1;
        if (button_draw(buf, b_back_to_main_menu) != 0)
            return 1;
        break;
    
    case WORD_SCREEN:
        if (vb_fill_screen(buf, MENU_BACKGROUND_COLOR) != 0)
            return 1;
        if (draw_game_correct_guess() != OK)
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
    if (dispatcher_bind_buttons(2, &b_new_game, &b_end_program) != 0)
        return 1;
    if (dispatcher_bind_text_boxes(0) != 0)
        return 1;
    dispatcher_bind_canvas(false);

    cursor_set_state(CURSOR_ARROW);
    menu_state = MAIN_MENU;
    return 0;
}

int menu_set_pause_menu() {
    if (dispatcher_bind_buttons(2, &b_resume, &b_back_to_main_menu) != 0)
        return 1;
    if (dispatcher_bind_text_boxes(0) != 0)
        return 1;
    dispatcher_bind_canvas(false);

    cursor_set_state(CURSOR_ARROW);
    menu_state = PAUSE_MENU;
    return 0;
}
