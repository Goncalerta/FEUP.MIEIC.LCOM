#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#include <lcom/lcf.h>
#include "keyboard.h"

/** @defgroup dispatcher dispatcher
 * @{
 *
 * @brief Module of the dispatcher.
 */

/**
 * @brief Resets all dispatcher bindings.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_reset_bindings();

/**
 * @brief Binds the given buttons to the dispatcher.
 * 
 * @param number_of_buttons number of buttons to bind
 * @param ... buttons to bind (button_t)
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_bind_buttons(size_t number_of_buttons, ...);

/**
 * @brief Binds the given text boxes to the dispatcher.
 * 
 * @param number_of_text_boxes number of text boxes to bind
 * @param ... text boxes to bind (text_box_t)
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatcher_bind_text_boxes(size_t number_of_text_boxes, ...);

/**
 * @brief Binds or unbinds the canvas to the dispatcher.
 * 
 * @param is_to_bind is canvas to bind
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatcher_bind_canvas(bool is_to_bind);

/**
 * @brief Dispatches a mouse packet.
 * 
 * @param p mouse packet
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatch_mouse_packet(struct packet p);

/**
 * @brief Dispatches a keyboard event.
 * 
 * @param kbd_event keyboard event
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatch_keyboard_event(kbd_event_t kbd_event);

/**
 * @brief Dispatches a timer interrupt.
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatch_timer_tick();

/**
 * @brief Dispatches a rtc alarm interrupt.
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatch_rtc_alarm_int();

/**
 * @brief Dispatches a rtc periodic interrupt.
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatch_rtc_periodic_int();

/**
 * @brief Draws the frame to the back buffer and flips the buffers.
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int draw_frame();

/**
 * @brief Updates the cursor state and updates the game according to it.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_update_cursor_state();

/**
 * @brief Reacts to the event that the other player opened the program.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_other_player_opened_program();

/**
 * @brief Notify the other player that this player left the game.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_notify_not_in_game();

/**
 * @brief Leaves the game.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_leave_game();

/**
 * @brief Reacts to the event that the other player left the game.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_other_player_leave_game();
int event_ready_to_play();
int event_other_player_ready_to_play();
int event_this_player_random_number();
int event_other_player_random_number();

int event_new_round_as_guesser(const char *word);
int event_new_round_as_drawer();

int event_start_round();
int event_end_round();

int event_new_stroke(bool primary_button);
int event_new_atom(uint16_t x, uint16_t y);
int event_undo();
int event_redo();

int event_guess_word(char *guess);
int event_round_win(uint32_t score);

/**
 * @brief Ends the program.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_end_program();

/**
 * @brief Checks if the program should end.
 * 
 * @return Return true if the program should end and false otherwise
 */
bool should_end();

/**@}*/

#endif /* __DISPATCHER_H */
