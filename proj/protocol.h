#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <lcom/lcf.h>

/** @file 
 * @brief File dedicated to the application dependent communication protocol between two computers.
 */


/** @defgroup protocol protocol
 * @{
 *
 * @brief Module dedicated to the application dependent communication protocol between two computers.
 */

/**
 * @brief Enumerated type for specifying the type of a message.
 * 
 */
typedef enum message_type_t {
    MSG_READY_TO_PLAY = 0, /*!< Notify player ready to play a new game. */
    MSG_LEAVE_GAME = 1, /*!< Notify player left the game. */
    MSG_RANDOM_NUMBER = 2, /*!< Send a random number. */
    MSG_NEW_ROUND = 3, /*!< Create a new round with a given correct guess. */
    MSG_START_ROUND = 4, /*!< Notify round start. */
    MSG_NEW_STROKE = 5, /*!< Notify new stroke being drawn. */
    MSG_DRAW_ATOM = 6, /*!< Notify new stroke atom drawn. */
    MSG_UNDO_CANVAS = 7, /*!< Notify canvas undo last stroke. */
    MSG_REDO_CANVAS = 8, /*!< Notify canvas redo stroke. */
    MSG_GUESS = 9, /*!< Make a guess for the correct word. */
    MSG_CLUE = 10, /*!< Show a hint to the word clue at a given position. */
    MSG_ROUND_WIN = 11, /*!< Notify round won, updating the score. */
    MSG_GAME_OVER = 12, /*!< Notify round is lost. */
    MSG_PROGRAM_OPENED = 13 /*!< Notify player opened the program. */
} message_type_t;

/**
 * @brief Represents a message sent to or received from the serial port.
 * 
 */
typedef struct message_t {
    message_type_t type; /*!< @brief Type of the message. */
    size_t content_len; /*!< @brief Number of bytes in the content of the message. */
    uint8_t *content; /*!< @brief Memory address to the content of the message. */
} message_t;

/**
 * @brief Configures the serial port and protocol queues to start communication.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_config_uart();

/**
 * @brief Frees the memory allocated for the protocol and serial port software queues. 
 *
 */
void protocol_exit();

/**
 * @brief Processes received bytes from the serial port.
 * 
 * Messages bytes are parsed into messages. Acknowledgments make the next pending message
 * available to be sent. Non-acknowledgments make the last sent message be repeated.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_handle_received_bytes();

/**
 * @brief Reacts to a serial port error.
 * 
 * Stops processing the current message and sends a non-acknowledgment to the other
 * computer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_handle_error();

/**
 * @brief Reacts to a timer interrupt to detect timeouts.
 * 
 * A timeout receiving a message leads to giving up on receiving that message and
 * sending a non-acknowledgment.
 * A timeout waiting for an acknowledgment leads to assuming the connection was lost,
 * giving up on all pending messages and notifying that the other player left the game.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_tick();

/**
 * @brief Sends a message of type MSG_READY_TO_PLAY.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_ready_to_play();

/**
 * @brief Sends a message of type MSG_LEAVE_GAME.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_leave_game();

/**
 * @brief Sends a message of type MSG_RANDOM_NUMBER.
 * 
 * @param random_number the random number being sent
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_random_number(int random_number);

/**
 * @brief Sends a message of type MSG_NEW_ROUND.
 * 
 * @param word the correct word being drawn in the new round
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_new_round(const char *word);

/**
 * @brief Sends a message of type MSG_START_ROUND.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_start_round();

/**
 * @brief Sends a message of type MSG_NEW_STROKE.
 * 
 * @param color the color of the stroke
 * @param thickness the thickness of the stroke
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_new_stroke(uint32_t color, uint16_t thickness);

/**
 * @brief Sends a message of type MSG_DRAW_ATOM.
 * 
 * @param x the x coordinate of the atom
 * @param y the y coordinate of the atom
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_new_atom(uint16_t x, uint16_t y);

/**
 * @brief Sends a message of type MSG_UNDO_CANVAS.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_undo_canvas();

/**
 * @brief Sends a message of type MSG_REDO_CANVAS.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_redo_canvas();

/**
 * @brief Sends a message of type MSG_GUESS.
 * 
 * @param guess the guess made
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_guess(const char *guess);

/**
 * @brief Sends a message of type MSG_CLUE.
 * 
 * @param pos the position of the character to be revealed in the word
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_clue(size_t pos);

/**
 * @brief Sends a message of type MSG_ROUND_WIN.
 * 
 * @param score the new score after winning the round
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_round_win(uint32_t score);

/**
 * @brief Sends a message of type MSG_GAME_OVER.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_game_over();

/**
 * @brief Sends a message of type MSG_PROGRAM_OPENED.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int protocol_send_program_opened();

/**@}*/

#endif /* _PROTOCOL_H */
