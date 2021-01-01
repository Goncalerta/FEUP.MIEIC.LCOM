#ifndef __MENU_H
#define __MENU_H

#include <lcom/lcf.h>
#include "game.h"
#include "keyboard.h"

/** @file 
 * @brief File dedicated to the interaction with the menu of the game as well as other program screens.
 */

/** @defgroup menu menu
 * @{
 *
 * @brief Module to interact with the menu of the game as well as other program screens.
 */

/**
 * @brief Enumerated type for specifying the state of the menu/screens being displayed.
 * 
 */
typedef enum menu_state_t {
    MAIN_MENU, /*!< Main menu. */
    DRAWER_NEW_ROUND_SCREEN, /*!< New round screen of drawer. */
    GUESSER_NEW_ROUND_SCREEN, /*!< New round screen of guesser. */
    GAME_OVER_SCREEN, /*!< Game over screen. */
    OTHER_PLAYER_LEFT_SCREEN, /*!< Game over because other player left screen. */
    GAME, /*!< Game is being played, unpaused. */
    PAUSE_MENU, /*!< Pause menu. */
    AWAITING_OTHER_PLAYER /*!< Awaiting other player menu. */
} menu_state_t;

/**
 * @brief Checks if a game is ongoing.
 * 
 * @return Return true if game is ongoing and false otherwise 
 */
bool menu_is_game_ongoing();

/**
 * @brief Loads the xpm images of the menu.
 * 
 * @param type type of xpm image
 * @return Return 0 upon success and non-zero otherwise
 */
int menu_init(enum xpm_image_type type);

/**
 * @brief Frees the space allocated in memory to store the menu xpm images.
 * 
 */
void menu_exit();

/**
 * @brief Updates the menu according to a keyboard event.
 * 
 * @param kbd_event keyboard event
 * @return Return 0 upon success and non-zero otherwise
 */
int menu_react_kbd(kbd_event_t kbd_event);

/**
 * @brief Draws the menu according to its state to the back buffer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int menu_draw();

/**
 * @brief Gets the current menu state.
 * 
 * @return Return the current menu state
 */
menu_state_t menu_get_state();

/**
 * @brief Sets the menu to the game screen.
 * 
 */
void menu_set_game_screen();

/**
 * @brief Sets the menu to main menu.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int menu_set_main_menu();

/**
 * @brief Sets the menu to pause menu.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int menu_set_pause_menu();

/**
 * @brief Sets the menu to awaiting other player menu.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int menu_set_awaiting_player_menu();

/**
 * @brief Sets the menu to new round screen according to the given player role.
 * 
 * @param role this player role
 * @return Return 0 upon success and non-zero otherwise
 */
int menu_set_new_round_screen(role_t role);

/**
 * @brief Sets the menu to the game over screen.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int menu_set_game_over_screen();

/**
 * @brief Sets the menu to the other player left screen.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int menu_set_other_player_left_screen();

/**@}*/

#endif /* __MENU_H */
