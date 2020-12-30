#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <lcom/lcf.h>

#define PROTOCOL_BIT_RATE 9600
#define PROTOCOL_ACK 0
#define PROTOCOL_NACK 1
#define PROTOCOL_WAIT_TIMEOUT_TICKS 90
#define PENDING_MESSAGES_CAPACITY 8

typedef enum message_type_t {
    MSG_READY_TO_PLAY = 0,
    MSG_LEAVE_GAME = 1,
    MSG_RANDOM_NUMBER = 2,
    MSG_NEW_ROUND = 3,
    MSG_START_ROUND = 4,
    MSG_NEW_STROKE = 5,
    MSG_DRAW_ATOM = 6,
    MSG_UNDO_CANVAS = 7,
    MSG_REDO_CANVAS = 8,
    MSG_GUESS = 9,
    MSG_CLUE = 10,
    MSG_ROUND_WIN = 11,
    MSG_GAME_OVER = 12,
    MSG_PROGRAM_OPENED = 13
} message_type_t;

typedef struct message_t {
    message_type_t type;
    size_t content_len;
    uint8_t *content;
} message_t;

int protocol_config_uart();
void protocol_exit();
int protocol_handle_received_bytes();
int protocol_handle_error();
int protocol_tick();

int protocol_send_ready_to_play();
int protocol_send_leave_game();
int protocol_send_random_number(int random_number);
int protocol_send_new_round(const char *word);
int protocol_send_start_round();
int protocol_send_new_stroke(uint32_t color, uint16_t thickness);
int protocol_send_new_atom(uint16_t x, uint16_t y);
int protocol_send_undo_canvas();
int protocol_send_redo_canvas();
int protocol_send_guess(const char *guess);
int protocol_send_clue(size_t pos);
int protocol_send_round_win(uint32_t score);
int protocol_send_game_over();
int protocol_send_program_opened();

#endif /* _PROTOCOL_H */
