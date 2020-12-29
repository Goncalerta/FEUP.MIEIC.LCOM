#ifndef __MENU_H
#define __MENU_H

#include <lcom/lcf.h>
#include "button.h"

typedef enum menu_state_t {
    MAIN_MENU,
    WORD_SCREEN,
    GAME_OVER_SCREEN,
    OTHER_PLAYER_LEFT_SCREEN,
    GAME,
    PAUSE_MENU,
    AWAITING_OTHER_PLAYER
} menu_state_t;

bool menu_is_game_ongoing();

int menu_init(enum xpm_image_type type);

int menu_draw();

menu_state_t menu_get_state();

void menu_set_state(menu_state_t state);

int menu_set_main_menu();

int menu_set_pause_menu();

int menu_set_awaiting_player_menu();

int menu_set_word_screen();

int menu_set_game_over_screen();

int menu_set_other_player_left_screen();

#endif /* __MENU_H */
