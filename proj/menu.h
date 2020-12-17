#ifndef __MENU_H
#define __MENU_H

#include <lcom/lcf.h>
#include "button.h"

//TODO maybe program_state_t? doesn't seem right to have "WORD_SCREEN" & "GAME" as menu states
// maybe merge menu_state_t with game_state_t to form a program_state_t ?

    // menu_state_t and game_state_t are two very different (and independent) things, they cannot be merged
    // menu_state_t says in which part of the menu the player is (this includes the game itself) [this is not synchronized between computers]
    // game_state_t says what's the state of the round (if it is ongoing, if the guess was correct, etc)  [this is synchronized between computers]
    // You may be ingame or paused both with round ongoing or gameover, they really are independent
    // I'm not necessarily againts naming it program_state_t but I personally think menu_state_t makes more sense
typedef enum menu_state_t {
    MAIN_MENU,
    WORD_SCREEN,
    GAME,
    PAUSE_MENU
} menu_state_t;


void menu_init(enum xpm_image_type type);

int menu_draw();

menu_state_t menu_get_state();

void menu_set_state(menu_state_t state);

int menu_set_main_menu();

int menu_set_pause_menu();

#endif /* __MENU_H */
