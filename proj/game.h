#ifndef __GAME_H
#define __GAME_H

#include <lcom/lcf.h>
#include "button.h"
#include "clue.h"
#include "textbox.h"

#define GAME_BAR_COLOR 0x00dddddd
#define GAME_BAR_COLOR_DARK 0x00aaaaaa
#define GAME_BAR_HEIGHT 150
#define GAME_BAR_PADDING 5
#define GAME_BAR_INNER_HEIGHT ((GAME_BAR_HEIGHT) - (GAME_BAR_PADDING))

#define MAX_GUESSES 5

typedef enum role_t {
    DRAWER,
    GUESSER
} role_t;

int game_load_assets(enum xpm_image_type type);
void game_unload_assets();
int new_game();
void delete_game();
int game_new_round(role_t role, const char *word);
void game_delete_round();
int game_resume();
int game_start_round();
int game_rtc_alarm();
int game_rtc_pi_tick();
int game_timer_tick();
int game_draw();
int game_give_clue();
int game_give_clue_at(size_t pos);
int game_round_over(uint32_t current_score, bool win);
int game_guess_word(char *guess);
int game_other_player_game_over();

void get_random_word(const char **word);

role_t game_get_role();
uint32_t game_get_score();
uint32_t game_get_round_number();
bool game_is_round_unstarted();
bool game_is_round_ongoing();
bool game_is_round_ongoing_or_tolerance();
bool game_is_round_won();
bool game_is_over();
void game_set_over();
bool game_may_create_new_round();
const char *game_get_correct_word();

int drawer_change_selected_color();
int drawer_change_selected_thickness();
uint32_t drawer_get_selected_color();
uint16_t drawer_get_selected_thickness();
int drawer_toggle_pencil_eraser();
int drawer_set_pencil_primary();
int drawer_set_eraser_primary();

#endif /* __GAME_H */
