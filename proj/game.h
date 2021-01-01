#ifndef __GAME_H
#define __GAME_H

#include <lcom/lcf.h>

/** @file 
 * @brief File dedicated to the game logic.
 */

/** @defgroup game game
 * @{
 *
 *  @brief Module dedicated to the game logic.
 */

#define GAME_BAR_HEIGHT 150 /**< @brief Game bar height */

/**
 * @brief Enumerated type for specifying the role of a player.
 * 
 */
typedef enum role_t {
    DRAWER, /*!< Player is the drawer: must draw a word. */
    GUESSER /*!< Player is the guesser: must guess the word drawn. */
} role_t;

/**
 * @brief Loads the game assets.
 * 
 * @param type type of xpm image
 * @return Return 0 upon success and non-zero otherwise
 */
int game_load_assets(enum xpm_image_type type);

/**
 * @brief Frees the space allocated in memory to store the game assets.
 * 
 */
void game_unload_assets();

/**
 * @brief Initiates a new game.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int new_game();

/**
 * @brief Deletes the current game.
 * 
 */
void delete_game();

/**
 * @brief Creates a new round.
 * 
 * @param role this player role
 * @param word round word to guess
 * @return Return 0 upon success and non-zero otherwise
 */
int game_new_round(role_t role, const char *word);

/**
 * @brief Deletes the current game round.
 * 
 */
void game_delete_round();

/**
 * @brief Resumes the game.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int game_resume();

/**
 * @brief Starts the already created round.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int game_start_round();

/**
 * @brief Updates the game due to a rtc alarm interrupt.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int game_rtc_alarm();

/**
 * @brief Updates the game due to a rtc periodic interrupt.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int game_rtc_pi_tick();

/**
 * @brief Updates the game due to a timer interrupt.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int game_timer_tick();

/**
 * @brief Draws the game screen to the back buffer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int game_draw();

/**
 * @brief Reveals another character in the word clue and notifies the other player.
 * 
 * Also sets the time for the next clue to be revealed.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int game_give_clue();

/**
 * @brief Reveals a character in the word clue at the given position.
 * 
 * @param pos position to give the clue at
 * @return Return 0 upon success and non-zero otherwise
 */
int game_give_clue_at(size_t pos);

/**
 * @brief Ends the current round.
 * 
 * @param current_score current game score
 * @param win was round won
 * @return Return 0 upon success and non-zero otherwise
 */
int game_round_over(uint32_t current_score, bool win);

/**
 * @brief Adds a word guess. And reacts according to weather it is correct.
 * 
 * The correct guess means the round is won. The wrong guess gives a time
 * penalty of 5 seconds.
 * 
 * @param guess address of memory of the guess
 * @return Return 0 upon success and non-zero otherwise
 */
int game_guess_word(char *guess);

/**
 * @brief Reacts to the game over notification of another player.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int game_other_player_game_over();

/**
 * @brief Gets a random word from the internal word list.
 * 
 * @return Return a random word from the internal word list
 */
const char *get_random_word();

/**
 * @brief Gets this player role.
 * 
 * @return Return this player role 
 */
role_t game_get_role();

/**
 * @brief Gets the game score.
 * 
 * @return Return the game score
 */
uint32_t game_get_score();

/**
 * @brief Gets the round number.
 * 
 * @return Return the round number
 */
uint32_t game_get_round_number();

/**
 * @brief Checks if the round is unstarted.
 * 
 * @return Return true if round is unstarted and false otherwise 
 */
bool game_is_round_unstarted();

/**
 * @brief Checks if the round is ongoing.
 * 
 * @return Return true if round is ongoing and false otherwise
 */
bool game_is_round_ongoing();

/**
 * @brief Checks if round is ongoing or time's up.
 * 
 * @return Return true if round is ongoing or time's up
 */
bool game_is_round_ongoing_or_tolerance();

/**
 * @brief Checks if the round is won.
 * 
 * @return Return true if round is won and false otherwise
 */
bool game_is_round_won();

/**
 * @brief Checks if the game is over.
 * 
 * @return Return true if game is over and false otherwise
 */
bool game_is_over();

/**
 * @brief Sets the game to over.
 * 
 */
void game_set_over();

/**
 * @brief Checks if a new round may be created.
 * 
 * @return Return true if a new round may be created and false otherwise
 */
bool game_may_create_new_round();

/**
 * @brief Gets the correct word of the round.
 * 
 * @return Return the address of memory of the correct word
 */
const char *game_get_correct_word();

/**
 * @brief Changes the selected color if this player is the drawer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int drawer_change_selected_color();

/**
 * @brief Changes the selected thickness if this player is the drawer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int drawer_change_selected_thickness();

/**
 * @brief Gets the drawer selected color.
 * 
 * @return Return the selected color
 */
uint32_t drawer_get_selected_color();

/**
 * @brief Gets the drawer selected thickness.
 * 
 * @return Return the selected thickness
 */
uint16_t drawer_get_selected_thickness();

/**
 * @brief Switches the tool used (pencil/eraser) if this player is the drawer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int drawer_toggle_pencil_eraser();

/**
 * @brief Sets the pencil primary if this player is the drawer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int drawer_set_pencil_primary();

/**
 * @brief Sets the eraser primary if this player is the drawer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int drawer_set_eraser_primary();

/**@}*/

#endif /* __GAME_H */
