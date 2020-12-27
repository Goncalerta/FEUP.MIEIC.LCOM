#ifndef __MENU_H
#define __MENU_H

#include <lcom/lcf.h>
#include "button.h"

typedef enum menu_state_t {
    MAIN_MENU,
    WORD_SCREEN,
    GAME,
    PAUSE_MENU,
    AWAITING_OTHER_PLAYER
} menu_state_t;


int menu_init(enum xpm_image_type type);

int menu_draw();

menu_state_t menu_get_state();

void menu_set_state(menu_state_t state);

int menu_set_main_menu();

int menu_set_pause_menu();

int menu_set_awaiting_player_menu();

#endif /* __MENU_H */
