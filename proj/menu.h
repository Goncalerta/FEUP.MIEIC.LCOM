#ifndef __MENU_H
#define __MENU_H

#include <lcom/lcf.h>
#include "button.h"

//TODO maybe program_state_t? doesn't seem right to have "WORD_SCREEN" & "GAME" as menu states
// maybe merge menu_state_t with game_state_t to form a program_state_t ?
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
