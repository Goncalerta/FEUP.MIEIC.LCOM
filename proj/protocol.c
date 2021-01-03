#include <lcom/lcf.h>

#include "uart.h"
#include "protocol.h"
#include "queue.h"
#include "dispatcher.h"
#include "canvas.h"
#include "game.h"

/** @defgroup protocol protocol
 * @{
 *
 */

#define PROTOCOL_BIT_RATE 9600 /**< @brief Communication bitrate */
#define PROTOCOL_ACK 0xAA /**< @brief Acknowledgment byte (everything OK) */
#define PROTOCOL_NACK 0x55 /**< @brief Non-acknowledgment byte (invalid message) */
#define PROTOCOL_MESSAGE_START_BYTE 0x0F /**< @brief Starting byte of a message */
#define PROTOCOL_MESSAGE_TRAILING_BYTE 0xF0 /**< @brief Trailing byte of a message */
#define PROTOCOL_WAIT_TIMEOUT_TICKS 90 /**< @brief Maximum seconds to timeout (receiving messages or acknowledgments) */
#define PROTOCOL_PING_FREQUENCY 300 /**< @brief Number of ticks between each ping to check if the other computer is running */
#define PENDING_MESSAGES_CAPACITY 8 /**< @brief Starting capacity of pending_messages queue */

static queue_t *pending_messages; /**< @brief Queue with pending messages to transmit */
static bool awaiting_ack = false; /**< @brief Whether this computer is awaiting and acknowledgment byte */
static uint8_t awaiting_ack_ticks = 0; /**< @brief Acknowlegment ticks to control timeout */
static uint16_t protocol_ping_ticks = 0; /**< @brief Ticks to control when the next ping should be sent */

/**
 * @brief Enumerated type for specifying the state of the message being received.
 * 
 */
typedef enum receiving_msg_state {
    NOT_RECEIVING_MESSAGE, /**< No message is being received */
    MESSAGE_START_BYTE_DETECTED, /**< Start byte of message detected */
    RECEIVING_MESSAGE_BODY, /**< Receiving message body */
    EXPECTING_TRAILING_BYTE /**< Message body fully received; expecting trailing byte */
} receiving_msg_state_t;

static receiving_msg_state_t receiving_msg = NOT_RECEIVING_MESSAGE; /**< @brief Current state of the message being received */
static uint8_t receiving_msg_ticks = 0; /**< @brief Receiving message ticks to control timeout */
static uint8_t *receiving_msg_bits = NULL; /**< @brief Received bytes of the message being received */
static size_t receiving_msg_len = 0; /**< @brief Number of bytes expected in the message being received */
static size_t receiving_msg_read_count = 0; /**< @brief Number of bytes of the message being received already read */

/**
 * @brief Handles a connection timeout (didn't receive acknowledgment byte)
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_handle_connection_timeout() {
    awaiting_ack = false;
    if (handle_other_player_leave_game() != OK)
        return 1;
    queue_empty(pending_messages);
    
    return 0;
}

/**
 * @brief Initializes a new message_t
 * 
 * @param message memory address of the message being initialized
 * @param type type of the message being initialized
 * @param content_len number of bytes in the message content
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_new_message(message_t *message, message_type_t type, size_t content_len, uint8_t *content) {
    message->type = type;
    message->content_len = content_len;
    message->content = malloc(content_len * sizeof(uint8_t));
    if (message->content == NULL)
        return 1;
    memcpy(message->content, content, content_len * sizeof(uint8_t));
    return 0;
}

/**
 * @brief Initializes a new message_t without content
 * 
 * @param message memory address of the message being initialized
 * @param type type of the message being initialized
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_new_message_no_content(message_t *message, message_type_t type) {
    message->type = type;
    message->content_len = 0;
    message->content = NULL;
    return 0;
}

/**
 * @brief Frees the resources of a message_t instance
 * 
 * @param message memory address of the message being deleted
 */
static void protocol_delete_message(message_t *message) {
    if (message->content_len != 0 && message->content != NULL) {
        free(message->content);
        message->content = NULL;
    }
}

/**
 * @brief Handles a received message of type MSG_READY_TO_PLAY.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_ready_to_play(size_t content_len, uint8_t *content) {
    if (content_len != 0)
            return 1;

    if (handle_other_player_ready_to_play() != OK)
        return 1;

    return 0;
}

/**
 * @brief Handles a received message of type MSG_LEAVE_GAME.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_leave_game(size_t content_len, uint8_t *content) {
    if (content_len != 0)
            return 1;

    if (handle_other_player_leave_game() != OK)
        return 1;

    return 0;
}

/**
 * @brief Handles a received message of type MSG_RANDOM_NUMBER.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_random_number(size_t content_len, uint8_t *content) {
    if (content_len != 4)
        return 1;

    int rn;
    memcpy(&rn, content, 4);

    if (handle_other_player_random_number(rn) != OK)
        return 1;
    
    return 0;
}

/**
 * @brief Handles a received message of type MSG_NEW_ROUND.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_new_round(size_t content_len, uint8_t *content) {
    if (content[content_len-1] != '\0')
        return 1;

    char *word;

    size_t word_len = strlen((char *) content) + 1;
    if (content_len != word_len)
        return 1;
    
    word = malloc(word_len * sizeof(char));
    if (word == NULL)
        return 1;

    strncpy(word, (char *) content, word_len);
    if (game_may_create_new_round()) {
        if (handle_new_round_as_guesser(word) != OK)
            return 1;
    } else if (game_is_over()) {
        if (handle_notify_not_in_game() != OK)
            return 1;
    }

    return 0;
}

/**
 * @brief Handles a received message of type MSG_START_ROUND.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_start_round(size_t content_len, uint8_t *content) {
    if (content_len != 0)
        return 1;

    if (game_is_round_unstarted() && game_get_role() == GUESSER) {
        if (handle_start_round() != OK)
            return 1;
    } else if (game_is_over()) {
        if (handle_notify_not_in_game() != OK)
            return 1;
    }
    
    return 0;
}

/**
 * @brief Handles a received message of type MSG_NEW_STROKE.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_new_stroke(size_t content_len, uint8_t *content) {
    if (content_len != 6)
        return 1;

    uint32_t color;
    uint16_t thickness;
    memcpy(&color, content, 4);
    memcpy(&thickness, content + 4, 2);

    if (canvas_is_initialized() && game_get_role() == GUESSER) {
        if (canvas_new_stroke(color, thickness) != OK)
            return 1;
    }
    
    return 0;
}

/**
 * @brief Handles a received message of type MSG_DRAW_ATOM.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_draw_atom(size_t content_len, uint8_t *content) {
    if (content_len != 4)
        return 1;

    uint16_t x, y;
    memcpy(&x, content, 2);
    memcpy(&y, content + 2, 2);

    if (canvas_is_initialized() && game_get_role() == GUESSER) {
        if (canvas_new_stroke_atom(x, y) != OK)
            return 1;
    }

    return 0;
}

/**
 * @brief Handles a received message of type MSG_UNDO_CANVAS.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_undo_canvas(size_t content_len, uint8_t *content) {
    if (content_len != 0)
        return 1;

    if (canvas_is_initialized() && game_get_role() == GUESSER) {
        if (canvas_undo_stroke() != OK)
            return 1;
    }

    return 0;
}

/**
 * @brief Handles a received message of type MSG_REDO_CANVAS.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_redo_canvas(size_t content_len, uint8_t *content) {
    if (content_len != 0)
        return 1;

    if (canvas_is_initialized() && game_get_role() == GUESSER) {
        if (canvas_redo_stroke() != OK)
            return 1;
    }

    return 0;
}

/**
 * @brief Handles a received message of type MSG_GUESS.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_guess(size_t content_len, uint8_t *content) {
    if (content[content_len-1] != '\0')
        return 1;

    char *guess;

    size_t guess_len = strlen((char *) content) + 1;
    if (content_len != guess_len)
        return 1;
    
    guess = malloc(guess_len * sizeof(char));
    if (guess == NULL)
        return 1;

    strncpy(guess, (char *) content, guess_len);
    if (game_is_round_ongoing_or_tolerance() && game_get_role() == DRAWER) {
        if (game_guess_word(guess) != OK)
            return 1;
    } else if (game_is_over()) {
        if (handle_notify_not_in_game() != OK)
            return 1;
    }

    return 0;
}

/**
 * @brief Handles a received message of type MSG_CLUE.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_clue(size_t content_len, uint8_t *content) {
    if (content_len != 1)
        return 1;

    uint8_t pos;
    memcpy(&pos, content, 1);

    if (game_is_round_ongoing() && game_get_role() == GUESSER) {
        if (game_give_clue_at(pos) != OK)
            return 1;
    } else if (game_is_over()) {
        if (handle_notify_not_in_game() != OK)
            return 1;
    }
    
    return 0;
}

/**
 * @brief Handles a received message of type MSG_ROUND_WIN.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_round_win(size_t content_len, uint8_t *content) {
    if (content_len != 4)
        return 1;

    uint32_t score;
    memcpy(&score, content, 4);
    if ((game_is_round_ongoing_or_tolerance() || game_is_round_won()) && game_get_role() == GUESSER) {
        if (game_round_over(score, true) != OK)
            return 1;
    } else if (game_is_over()) {
        if (handle_notify_not_in_game() != OK)
            return 1;
    }

    return 0;
}

/**
 * @brief Handles a received message of type MSG_GAME_OVER.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_game_over(size_t content_len, uint8_t *content) {
    if (content_len != 0)
        return 1;

    if (game_is_round_ongoing_or_tolerance()) {
        if (game_other_player_game_over() != OK)
            return 1;
    } else if (game_is_over()) {
        if (handle_notify_not_in_game() != OK)
            return 1;
    }

    return 0;
}

/**
 * @brief Handles a received message of type MSG_PROGRAM_OPENED.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_program_opened(size_t content_len, uint8_t *content) {
    if (content_len != 0)
        return 1;

    // In case the this computer thought the other one was already running
    // so that the connection can be reset.
    if (protocol_handle_connection_timeout() != OK)
        return 1;
    if (handle_other_player_opened_program() != OK)
        return 1;

    return 0;
}

/**
 * @brief Handles a received message of type MSG_PING.
 * 
 * @param content_len number of bytes in the content of the message
 * @param content memory address to the content of the message
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_receive_ping(size_t content_len, uint8_t *content) {
    if (content_len != 0)
        return 1;

    // Pings don't have to be handled. Do nothing.

    return 0;
}

typedef int (*message_handle_t)(size_t, uint8_t *); /**< @brief Message handler for a given type function pointer */
#define NUMBER_OF_MESSAGES 15 /**< @brief Number of possible messages (and thus message handlers) */
/**
 * @brief Array of message handlers, indexed by message type
 * 
 */
static const message_handle_t message_handle[NUMBER_OF_MESSAGES] = {
    protocol_receive_ready_to_play,
    protocol_receive_leave_game,
    protocol_receive_random_number,
    protocol_receive_new_round,
    protocol_receive_start_round,
    protocol_receive_new_stroke,
    protocol_receive_draw_atom,
    protocol_receive_undo_canvas,
    protocol_receive_redo_canvas,
    protocol_receive_guess,
    protocol_receive_clue,
    protocol_receive_round_win,
    protocol_receive_game_over,
    protocol_receive_program_opened,
    protocol_receive_ping
};

/**
 * @brief Sends the next pending message in queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_send_next_message() {
    if (queue_is_empty(pending_messages))
        return 0;
    
    message_t msg;
    if (queue_top(pending_messages, &msg) != OK)
        return 1;
    
    if (uart_send_byte(PROTOCOL_MESSAGE_START_BYTE) != OK)
        return 1;

    if (uart_send_byte(msg.content_len + 1) != OK) // size = type byte + content_len
        return 1;

    if (uart_send_byte(msg.type) != OK)
        return 1;
    
    for (uint8_t i = 0; i < msg.content_len; i++) {
        if (uart_send_byte(msg.content[i]) != OK)
            return 1;
    }

    if (uart_send_byte(PROTOCOL_MESSAGE_TRAILING_BYTE) != OK)
        return 1;

    awaiting_ack = true;
    awaiting_ack_ticks = 0;

    return 0;
}

/**
 * @brief Adds a new message to the pending messages queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_add_message(message_t message) {
    if (queue_push(pending_messages, &message) != OK)
        return 1;

    if (!awaiting_ack) {
        if (protocol_send_next_message() != OK)
            return 1;
    }
    return 0;
}

/**
 * @brief Handles a received acknowledgment byte.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_handle_ack() {
    message_t msg;
    
    if (awaiting_ack) {
        if (queue_top(pending_messages, &msg) != OK)
            return 1;
        
        protocol_delete_message(&msg);

        if (queue_pop(pending_messages) != OK)
            return 1;
    
        awaiting_ack = false;

        if (protocol_send_next_message() != OK)
            return 1;
    }
    
    return 0;
}

/**
 * @brief Handles a received non-acknowledgment byte.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_handle_nack() {
    if (awaiting_ack) {
        if (protocol_send_next_message() != OK)
            return 1;
    }
    
    return 0;
}

/**
 * @brief Handles a received byte when no message is currently being received.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_handle_byte_not_receiving_message() {
    uint8_t byte;
    if (uart_read_byte(&byte) != OK)
        return 1;

    switch (byte) {
    case PROTOCOL_ACK:
        if (protocol_handle_ack() != OK)
            return 1;
        break;
    case PROTOCOL_NACK:
        if (protocol_handle_nack() != OK)
            return 1;
        break;
    case PROTOCOL_MESSAGE_START_BYTE:
        receiving_msg = MESSAGE_START_BYTE_DETECTED;
        break;
    default:
        // Unexpected byte. Might be from a process different from this program
        // That uses a different protocol. Safer to ignore than to try to communicate back.
        break;
    }

    return 0;
}

/**
 * This byte tells the lenght of the body of the message.
 * 
 * @brief Handles a received byte after the starting message byte.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_handle_new_msg() {
    uint8_t byte;
    if (uart_read_byte(&byte) != OK)
        return 1;
    
    if (byte == 0) {
        // Message should have at least one byte
        if (protocol_handle_error() != OK)
            return 1;
        return 0;
    }

    receiving_msg = RECEIVING_MESSAGE_BODY;
    receiving_msg_ticks = 0;
    receiving_msg_len = byte;
    receiving_msg_read_count = 0;
    receiving_msg_bits = malloc(receiving_msg_len * sizeof(uint8_t));
    if (receiving_msg_bits == NULL)
        return 1;

    return 0;
}

/**
 * @brief Handles a received byte that is expected to belong to the message body.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_handle_message_body() {
    while (uart_received_bytes() && receiving_msg_read_count < receiving_msg_len) {
        uint8_t byte;
        if (uart_read_byte(&byte) != OK)
            return 1;
        receiving_msg_bits[receiving_msg_read_count] = byte;
        receiving_msg_read_count++;
    }

    if (receiving_msg_read_count >= receiving_msg_len) {
        receiving_msg = EXPECTING_TRAILING_BYTE;
    }

    return 0;
}

/**
 * @brief Parse and handle the received message.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_parse_received_message() {
    message_t msg;
    if (protocol_new_message(&msg, receiving_msg_bits[0], receiving_msg_len - 1, receiving_msg_bits + 1) != OK)
        return 1;

    if (msg.type >= NUMBER_OF_MESSAGES)
        return 1;

    if (message_handle[msg.type](msg.content_len, msg.content) != OK) {
        protocol_delete_message(&msg);
        return 1;
    }

    protocol_delete_message(&msg);
    
    return 0;
}

/**
 * @brief Handle a received byte when a trailing message byte is expected.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int protocol_handle_message_trailing_byte() {
    uint8_t byte;
    if (uart_read_byte(&byte) != OK)
        return 1;

    if (byte != PROTOCOL_MESSAGE_TRAILING_BYTE) {
        if (protocol_handle_error() != OK)
            return 1;
    }

    if (protocol_parse_received_message() == OK) {
        if (uart_send_byte(PROTOCOL_ACK) != OK)
            return 1;
    } else {
        if (uart_send_byte(PROTOCOL_NACK) != OK)
            return 1;
    }
    
    receiving_msg = NOT_RECEIVING_MESSAGE;
    if (receiving_msg_bits != NULL) {
        free(receiving_msg_bits);
        receiving_msg_bits = NULL;
    }

    return 0;
}

int protocol_handle_error() {
    if (receiving_msg_bits != NULL) {
        free(receiving_msg_bits);
        receiving_msg_bits = NULL;
    }

    uint8_t first, last, no_bytes;
    
    if (uart_flush_received_bytes(&no_bytes, &first, &last) != OK)
        return 1;
    
    // Try to detect acknowledgements that may have been sent before or after the message
    if (awaiting_ack && no_bytes > 0) {
        if (no_bytes > 1 && receiving_msg == NOT_RECEIVING_MESSAGE) {
            if (first == PROTOCOL_ACK) {
                if (protocol_handle_ack() != OK)
                    return 1;
            } else if (first == PROTOCOL_NACK) {
                if (protocol_handle_nack() != OK)
                    return 1;
            } else if (last == PROTOCOL_ACK) {
                if (protocol_handle_ack() != OK)
                    return 1;
            } else if (last == PROTOCOL_NACK) {
                if (protocol_handle_nack() != OK)
                    return 1;
            }
        } else {
            if (last == PROTOCOL_ACK) {
                if (protocol_handle_ack() != OK)
                    return 1;
            } else if (last == PROTOCOL_NACK) {
                if (protocol_handle_nack() != OK)
                    return 1;
            }
        }
    }

    if (uart_send_byte(PROTOCOL_NACK) != OK)
        return 1;

    receiving_msg = NOT_RECEIVING_MESSAGE;
    
    return 0;
}

int protocol_handle_received_bytes() {
    while (uart_received_bytes()) {
        switch (receiving_msg) {
        case NOT_RECEIVING_MESSAGE:
            if (protocol_handle_byte_not_receiving_message() != OK)
                return 1;
            break;
        case MESSAGE_START_BYTE_DETECTED:
            if (protocol_handle_new_msg() != OK)
                return 1;
            break;
        case RECEIVING_MESSAGE_BODY:
            if (protocol_handle_message_body() != OK)
                return 1;
            break;
        case EXPECTING_TRAILING_BYTE:
            if (protocol_handle_message_trailing_byte() != OK)
                return 1;
            break;
        default:
            return 1;
        }
    }

    return 0;    
}

int protocol_config_uart() {
    pending_messages = new_queue(sizeof(message_t), PENDING_MESSAGES_CAPACITY);
    if (pending_messages == NULL)
        return 1;
    
    awaiting_ack = false;
    receiving_msg = NOT_RECEIVING_MESSAGE;

    if (uart_init_sw_queues() != OK)
        return 1;
    if (uart_config_params(WORD_LEN_8_BITS, PARITY_ODD, STOP_BITS_2, PROTOCOL_BIT_RATE) != OK)
        return 1;
    if (uart_config_int(true, true, true) != OK)
        return 1;
    if (uart_enable_fifo(FIFO_8_BYTES) != OK)
        return 1;
    if (uart_clear_hw_fifos() != OK)
        return 1;
    
    return 0;
}

void protocol_exit() {
    delete_queue(pending_messages);
    if (receiving_msg_bits != NULL) {
        free(receiving_msg_bits);
        receiving_msg_bits = NULL;
    }
    uart_delete_sw_queues();
}

int protocol_tick() {
    protocol_ping_ticks++;
    if (protocol_ping_ticks >= PROTOCOL_PING_FREQUENCY) {
        protocol_send_ping();
        protocol_ping_ticks = 0;
    }

    if (awaiting_ack) {
        awaiting_ack_ticks++;

        if (awaiting_ack_ticks > PROTOCOL_WAIT_TIMEOUT_TICKS) {
            if (protocol_handle_connection_timeout() != OK)
                return 1;
        }    
    }

    if (receiving_msg != NOT_RECEIVING_MESSAGE) {
        receiving_msg_ticks++;

        if (receiving_msg_ticks > PROTOCOL_WAIT_TIMEOUT_TICKS) {
            if (protocol_handle_error() != OK)
                return 1;
        }
    }

    return 0;
}

int protocol_send_ready_to_play() {
    message_t msg;
    if (protocol_new_message_no_content(&msg, MSG_READY_TO_PLAY) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_leave_game() {
    message_t msg;
    if (protocol_new_message_no_content(&msg, MSG_LEAVE_GAME) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_random_number(int random_number) {
    message_t msg;
    uint8_t content[4];
    memcpy(content, &random_number, 4);

    if (protocol_new_message(&msg, MSG_RANDOM_NUMBER, 4, content) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_new_round(const char *word) {
    message_t msg;
    size_t str_len = strlen(word);
    if (str_len > 254) str_len = 254;
    
    uint8_t content[str_len + 1];
    memcpy(content, word, str_len + 1);

    if (protocol_new_message(&msg, MSG_NEW_ROUND, str_len + 1, content) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_start_round() {
    message_t msg;
    if (protocol_new_message_no_content(&msg, MSG_START_ROUND) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_new_stroke(uint32_t color, uint16_t thickness) {
    message_t msg;
    uint8_t content[6];
    memcpy(content, &color, 4);
    memcpy(content + 4, &thickness, 2);

    if (protocol_new_message(&msg, MSG_NEW_STROKE, 6, content) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_new_atom(uint16_t x, uint16_t y) {
    message_t msg;
    uint8_t content[4];
    memcpy(content, &x, 2);
    memcpy(content + 2, &y, 2);

    if (protocol_new_message(&msg, MSG_DRAW_ATOM, 4, content) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_undo_canvas() {
    message_t msg;
    if (protocol_new_message_no_content(&msg, MSG_UNDO_CANVAS) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_redo_canvas() {
    message_t msg;
    if (protocol_new_message_no_content(&msg, MSG_REDO_CANVAS) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_guess(const char *guess) {
    message_t msg;
    size_t str_len = strlen(guess);
    if (str_len > 254) str_len = 254;
    
    uint8_t content[str_len + 1];
    memcpy(content, guess, str_len + 1);

    if (protocol_new_message(&msg, MSG_GUESS, str_len + 1, content) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_clue(size_t pos) {
    uint8_t pos_8b = pos; // One byte is enough to encode the position
    message_t msg;
    uint8_t content[1];
    memcpy(content, &pos_8b, 1);

    if (protocol_new_message(&msg, MSG_CLUE, 1, content) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_round_win(uint32_t score) {
    message_t msg;
    uint8_t content[4];
    memcpy(content, &score, 4);

    if (protocol_new_message(&msg, MSG_ROUND_WIN, 4, content) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_game_over() {
    message_t msg;
    if (protocol_new_message_no_content(&msg, MSG_GAME_OVER) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_program_opened() {
    message_t msg;
    if (protocol_new_message_no_content(&msg, MSG_PROGRAM_OPENED) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

int protocol_send_ping() {
    message_t msg;
    if (protocol_new_message_no_content(&msg, MSG_PING) != OK)
        return 1;

    if (protocol_add_message(msg) != OK)
        return 1;

    return 0;
}

/**@}*/
