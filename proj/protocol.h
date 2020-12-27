#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <lcom/lcf.h>

/* TODO Which one should we use? 1200, 2400, 4800, 9600, 19200, 38400, 57600 or 115200 */
#define PROTOCOL_BIT_RATE 9600
#define PROTOCOL_ACK 0
#define PROTOCOL_NACK 1
#define PROTOCOL_WAIT_TIMEOUT_TICKS 150
#define PENDING_MESSAGES_CAPACITY 8

typedef enum message_type_t {
    MSG_READY_TO_PLAY = 0,
    MSG_LEAVE_GAME = 1,
    MSG_RANDOM_NUMBER = 2,
    MSG_NEW_ROUND = 3,

    // MSG_NEW_STROKE = 1,
    // MSG_DRAW_ATOM = 2,
    // MSG_UNDO_CANVAS = 3,
    // MSG_REDO_CANVAS = 4,
    // MSG_SYNC_TIMER = 5,
    // MSG_MAKE_GUESS = 6,
    // MSG_ROUND_WIN = 7,
    // MSG_GAME_OVER = 8,
    // MSG_START_ROUND = 9,
    // MSG_READY_TO_PLAY = 10,
    // MSG_ADD_CLUE = 11,
} message_type_t;

typedef struct message_t {
    message_type_t type;
    size_t content_len;
    uint8_t *content;
} message_t;

int protocol_config_uart();
int protocol_handle_received_bytes();
int protocol_handle_error();
int protocol_tick();

int protocol_send_ready_to_play();
int protocol_send_leave_game();
int protocol_send_random_number(int random_number);
int protocol_send_new_round(uint32_t round_number, const char *word);

#endif /* _PROTOCOL_H */
