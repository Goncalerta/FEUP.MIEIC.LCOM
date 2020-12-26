#include <lcom/lcf.h>

#include "uart.h"
#include "protocol.h"
#include "queue.h"

static queue_t pending_messages;
static bool awaiting_ack;
static uint8_t awaiting_ack_ticks;

static bool receiving_msg;
static uint8_t receiving_msg_ticks;
static uint8_t *receiving_msg_bits;
static size_t receiving_msg_len;
static size_t receiving_msg_read_count;

static int new_message(message_t *message, message_type_t type, size_t content_len, uint8_t *content) {
    message->type = type;
    message->content_len = content_len;
    message->content = malloc(content_len * sizeof(uint8_t));
    if (message->content == NULL)
        return 1;
    memcpy(message->content, content, content_len * sizeof(uint8_t));
    return 0;
}

static int delete_message(message_t *message) {
    if (message->content == NULL)
        return 1;
    free(message->content);
    message->content = NULL;
    return 0;
}

static int protocol_send_next_message() {
    if (queue_is_empty(&pending_messages))
        return 0;
    
    message_t msg;
    if (queue_top(&pending_messages, &msg) != OK)
        return 1;
    
    if (uart_send_byte(msg.content_len + 2) != OK)
        return 1;

    if (uart_send_byte(msg.type) != OK)
        return 1;
    
    for (uint8_t i = 0; i < msg.content_len; i++) {
        if (uart_send_byte(msg.content[i]) != OK)
            return 1;
    }

    // TODO remove
    printf("SENDING msg: 0x%x, len: %d\n[", msg.type, msg.content_len);
    for (size_t i = 0; i < msg.content_len; i++) {
        printf("0x%x, ", msg.content[i]);
    }
    printf("]\n");

    awaiting_ack = true;
    awaiting_ack_ticks = 0;

    return 0;
}

static int protocol_add_message(message_t message) {
    if (queue_push(&pending_messages, &message) != OK)
        return 1;

    if (!awaiting_ack) {
        if (protocol_send_next_message() != OK)
            return 1;
    }
    return 0;
}

static int protocol_handle_ack() {
    message_t msg;
    
    if (awaiting_ack) {
        if (queue_top(&pending_messages, &msg) != OK)
            return 1;
        
        if (delete_message(&msg) != OK)
            return 1;

        if (queue_pop(&pending_messages) != OK)
            return 1;
    
        awaiting_ack = false;

        if (protocol_send_next_message() != OK)
            return 1;
    }
    
    return 0;
}

static int protocol_handle_nack() {
    if (awaiting_ack) {
        if (protocol_send_next_message() != OK)
            return 1;
    }
    
    return 0;
}

static int protocol_handle_new_msg(uint8_t byte) {
    receiving_msg = true;
    receiving_msg_ticks = 0;
    receiving_msg_len = byte;
    receiving_msg_read_count = 1;
    receiving_msg_bits = malloc(receiving_msg_len * sizeof(uint8_t));
    if (receiving_msg_bits == NULL)
        return 1;

    return 0;
}

static int protocol_parse_received_message() {
    message_t msg;
    if (new_message(&msg, receiving_msg_bits[0], receiving_msg_len - 2, receiving_msg_bits + 1) != OK)
        return 1;

    // TODO dispatch message
    printf("msg: 0x%x, len: %d\n[", msg.type, msg.content_len);
    for (size_t i = 0; i < msg.content_len; i++) {
        printf("0x%x, ", msg.content[i]);
    }
    printf("]\n");

    return 0;
}

static int protocol_handle_message_body() {
    while (uart_received_bytes() && receiving_msg_read_count < receiving_msg_len) {
        uint8_t byte;
        if (uart_read_byte(&byte) != OK)
            return 1;
        receiving_msg_bits[receiving_msg_read_count-1] = byte;
        receiving_msg_read_count++;
    }

    if (receiving_msg_read_count >= receiving_msg_len) {
        if (protocol_parse_received_message() == OK) {
            printf("SENDING ACK\n");
            if (uart_send_byte(PROTOCOL_ACK) != OK)
                return 1;
        } else {
            printf("SENDING NACK\n");
            if (uart_send_byte(PROTOCOL_NACK) != OK)
                return 1;
        }
        
        receiving_msg = false;
        free(receiving_msg_bits);
    }

    return 0;
}

int protocol_handle_error() {
    receiving_msg = false;
    free(receiving_msg_bits);

    uint8_t first, last, no_bytes;
    
    if (uart_flush_received_bytes(&no_bytes, &first, &last) != OK)
        return 1;
    
    if (awaiting_ack && no_bytes > 0) {
        if (no_bytes > 1 && !receiving_msg) {
            if (first == PROTOCOL_ACK) {
                printf("RECEIVED ACK\n");
                if (protocol_handle_ack() != OK)
                    return 1;
            } else if (first == PROTOCOL_NACK) {
                printf("RECEIVED NACK\n");
                if (protocol_handle_nack() != OK)
                    return 1;
            } else if (last == PROTOCOL_ACK) {
                printf("RECEIVED ACK\n");
                if (protocol_handle_ack() != OK)
                    return 1;
            } else if (last == PROTOCOL_NACK) {
                printf("RECEIVED NACK\n");
                if (protocol_handle_nack() != OK)
                    return 1;
            }
        } else {
            if (last == PROTOCOL_ACK) {
                printf("RECEIVED ACK\n");
                if (protocol_handle_ack() != OK)
                    return 1;
            } else if (last == PROTOCOL_NACK) {
                printf("RECEIVED NACK\n");
                if (protocol_handle_nack() != OK)
                    return 1;
            }
        }
    }

    printf("HE SENDING NACK\n");
    if (uart_send_byte(PROTOCOL_NACK) != OK)
        return 1;
    
    return 0;
}

int protocol_handle_received_bytes() {
    while (uart_received_bytes()) {
        if (receiving_msg) {
            if (protocol_handle_message_body() != OK)
                return 1;
        } else {
            uint8_t byte;
            if (uart_read_byte(&byte) != OK)
                return 1;

            switch (byte) {
            case PROTOCOL_ACK:
                printf("RECEIVED ACK\n");
                if (protocol_handle_ack() != OK)
                    return 1;
                break;
            case PROTOCOL_NACK:
                printf("RECEIVED NACK\n");
                if (protocol_handle_nack() != OK)
                    return 1;
                break;
            default:
                if (protocol_handle_new_msg(byte) != OK)
                    return 1;
                if (awaiting_ack_ticks >= PROTOCOL_WAIT_TIMEOUT_TICKS) {
                    if (protocol_send_next_message() != OK)
                        return 1;
                }
                break;
            }
        }
    }

    return 0;    
}

int protocol_config_uart() {
    if (new_queue(&pending_messages, sizeof(message_t), PENDING_MESSAGES_CAPACITY) != OK)
        return 1;
    
    awaiting_ack = false;
    receiving_msg = false;

    if (uart_init_sw_queues() != OK)
        return 1;
    if (uart_config_params(WORD_LEN_8_BYTES, PARITY_ODD, STOP_BITS_2, PROTOCOL_BIT_RATE) != OK)
        return 1;
    if (uart_config_int(true, true, true) != OK)
        return 1;
    if (uart_enable_fifo(FIFO_8_BYTES) != OK)
        return 1;
    if (uart_clear_hw_fifos() != OK)
        return 1;
    uart_flush_RBR();
    uint8_t noop;
    uart_flush_received_bytes(&noop, &noop, &noop);

    
    return 0;
}

int protocol_tick() {
    if (awaiting_ack && awaiting_ack_ticks < PROTOCOL_WAIT_TIMEOUT_TICKS) {
        awaiting_ack_ticks++;
    }

    if (receiving_msg) {
        receiving_msg_ticks++;

        if (receiving_msg_ticks > PROTOCOL_WAIT_TIMEOUT_TICKS) {
            if (protocol_handle_error() != OK)
                return 1;
        }
    }

    return 0;
}

// TODO delete
void protocol_test_msg() {
    uint8_t content[3];
    content[0] = 0xAB;
    content[1] = 0x32;
    content[2] = 0xFE;
    message_t msg;
    if (new_message(&msg, 7, 3, content) != OK)
        return;
    protocol_add_message(msg);
}

void protocol_test_msg2() {
    uint8_t content[7];
    content[0] = 0x00;
    content[1] = 0x01;
    content[2] = 0x00;
    content[3] = 0x02;
    content[4] = 0x00;
    content[5] = 0x03;
    content[6] = 0x00;
    message_t msg;
    if (new_message(&msg, 0x77, 7, content) != OK)
        return;
    protocol_add_message(msg);
}
