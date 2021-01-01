#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#include <lcom/lcf.h>
#include "keyboard.h"
#include "protocol.h"

/** @file 
 * @brief File dedicated to handling and dispatching events.
 */

/** @defgroup dispatcher dispatcher
 * @{
 *
 * @brief Module dedicated to handling and dispatching events.
 */

/**
 * @brief Enumerated type for specifying the type of an event.
 * 
 */
typedef enum event_type_t {
    MOUSE_EVENT, /*!< Event from mouse. */
    KEYBOARD_EVENT, /*!< Event from keyboard. */
    RTC_PERIODIC_INTERRUPT_EVENT, /*!< Event from rtc periodic interrupts. */
    RTC_ALARM_EVENT, /*!< Event from rtc alarm. */
    UART_MESSAGE_EVENT, /*!< Event from serial port message. */
    TIMER_TICK_EVENT /*!< Event from timer interrupt. */
} event_type_t;

/**
 * @brief Content of an event.
 * 
 */
typedef union event_content_t {
    struct packet mouse_packet; /*!< @brief Mouse packet from a mouse event. */
    kbd_event_t kbd_state; /*!< @brief Keyboard state from a keyboard event. */
    message_t uart_message; /*!< @brief Message from a serial port message event. */
    void *no_content; /*!< @brief Set for events without content. */
} event_content_t;

/**
 * @brief Represents an event.
 * 
 */
typedef struct event_t {
    event_type_t type; /*!< @brief The type of the event. */
    event_content_t content; /*!< @brief The content of the event. */
} event_t;

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
 * @brief Initializes the dispatcher, creating an event queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_init();

/**
 * @brief Deletes all resources allocated to the dispatcher.
 * 
 */
void dispatcher_exit();

/**
 * @brief Retrieves mouse packet and adds a mouse event to the event queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_queue_mouse_event();

/**
 * @brief Retrieves keyboard state and adds a keyboard event to the event queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_queue_keyboard_event();

/**
 * @brief Adds an rtc periodic interrupt event to the event queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_queue_rtc_periodic_interrupt_event();

/**
 * @brief Adds an rtc alarm interrupt event to the event queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_queue_rtc_alarm_event();

/**
 * @brief Adds a uart message event to the event queue with the given message as content.
 * 
 * @param message message that is the content of this event
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_queue_uart_message_event(message_t message);

/**
 * @brief Adds a timer interrupt event to the event queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_queue_timer_tick_event();

/**
 * @brief Dispatches all events currently in the event queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int dispatcher_dispatch_events();

/**
 * @brief Dispatches a mouse packet from a mouse event.
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
 * @brief Dispatches a rtc periodic interrupt.
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatch_rtc_periodic_int();

/**
 * @brief Dispatches a rtc alarm interrupt.
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatch_rtc_alarm_int();

/**
 * @brief Dispatches a uart message.
 * 
 * @param message message to dispatch
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatch_uart_message(message_t message);

/**
 * @brief Dispatches a timer interrupt.
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int dispatch_timer_tick();

/**
 * @brief Draws the frame to the back buffer and flips the buffers.
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int draw_frame();

/**
 * @brief Updates the cursor state and updates the game according to it.
 * 
 * Used when dispatching mouse events or when the screen changes.
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
 * @brief Notify the other player that this player is not currently in the game.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_notify_not_in_game();

/**
 * @brief Leaves the game back to the main menu and notifies the other player.
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

/**
 * @brief Goes into the "Awaiting other player..." screen and notifies the other player.
 * 
 * If the other player is ready, generates and sends a random number to decide starting roles.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_ready_to_play();

/**
 * @brief Reacts to the event that the other player is ready to play.
 * 
 * If this player is also ready, generates and sends a random number to decide starting roles.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_other_player_ready_to_play();

/**
 * @brief Generates and sends a random number to the other player to decide starting roles.
 * 
 * Both computers compare the sent and received random numbers before starting the game.
 * The computer that generated the highest number will have the role (role_t) of DRAWER on the first round.
 * In the extremely unlikely case that both players generate the same random number, a new pair is generated and
 * sent between them.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_this_player_random_number();

/**
 * @brief Reacts to the event that the other player sent the random number to decide starting roles.
 * 
 * Both computers compare the sent and received random numbers before starting the game.
 * The computer that generated the highest number will have the role (role_t) of DRAWER on the first round.
 * In the extremely unlikely case that both players generate the same random number, a new pair is generated and
 * sent between them.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_other_player_random_number();

/**
 * @brief Starts a new round as GUESSER (role_t).
 * 
 * @param word the correct guess for this round.
 * @return Return 0 upon success and non-zero otherwise
 */
int event_new_round_as_guesser(const char *word);

/**
 * @brief Starts a new round as DRAWER (role_t).
 * 
 * The computer with this role is the one that mainly controls the round,
 * generating the correct word and setting the time to start the round.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_new_round_as_drawer();

/**
 * @brief Starts the round.
 * 
 * The round should have already been created (event_new_round_as_guesser() or event_new_round_as_drawer()) before starting.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_start_round();

/**
 * @brief Ends the current round.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_end_round();

/**
 * @brief Starts drawing new stroke in the canvas and notifies the other player.
 * 
 * The color and thickness selected is retrieved from the game state.
 * Only used by the player with the role DRAWER (role_t).
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int event_new_stroke();

/**
 * @brief Adds a new atom to the stroke being drawn in the canvas and notifies the other player.
 * 
 * @param x The x coordinate of the atom.
 * @param y The y coordinate of the atom.
 * @return Return 0 upon success and non-zero otherwise
 */
int event_new_atom(uint16_t x, uint16_t y);

/**
 * @brief Undoes a canvas stroke and notifies the other player.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int event_undo();

/**
 * @brief Redoes a canvas stroke and notifies the other player.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int event_redo();

/**
 * @brief Tries to guess the word and notifies the other player.
 * 
 * This event is triggered as a callback of the GUESSER's (role_t) text box
 * when the text is submitted.
 * 
 * @param guess the word guessed.
 * @return Return 0 upon success and non-zero otherwise
 */
int event_guess_word(char *guess);

/**
 * @brief Sets the round as won, updates the score and notifies the other player.
 * 
 * This event is triggered by the DRAWER (role_t) when the correct guess is received
 * from the GUESSER.
 * 
 * @param score the new score after winning the round.
 * @return Return 0 upon success and non-zero otherwise
 */
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
