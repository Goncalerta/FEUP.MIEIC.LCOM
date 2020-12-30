#ifndef __MENU_H
#define __MENU_H

#include <lcom/lcf.h>
#include "button.h"
#include "game.h"

typedef enum menu_state_t {
    MAIN_MENU,
    DRAWER_NEW_ROUND_SCREEN,
    GUESSER_NEW_ROUND_SCREEN,
    GAME_OVER_SCREEN,
    OTHER_PLAYER_LEFT_SCREEN,
    GAME,
    PAUSE_MENU,
    AWAITING_OTHER_PLAYER
} menu_state_t;

bool menu_is_game_ongoing();

int menu_init(enum xpm_image_type type);
void menu_exit();

int menu_draw();

menu_state_t menu_get_state();

void menu_set_state(menu_state_t state);

int menu_set_main_menu();

int menu_set_pause_menu();

int menu_set_awaiting_player_menu();

int menu_set_new_round_screen(role_t role);

int menu_set_game_over_screen();

int menu_set_other_player_left_screen();

#endif /* __MENU_H */
