#ifndef _UART_H_
#define _UART_H_

#include <lcom/lcf.h>

#define COM1_BASE_ADDR 0x3F8
#define COM1_IRQ 4
#define COM2_BASE_ADDR 0x2F8
#define COM2_IRQ 3

#define RECEIVER_BUFFER_REG 0
#define TRANSMITTER_HOLDING_REG 0
#define INTERRUPT_ENABLE_REG 1
#define INTERRUPT_IDENTIFICATION_REG 2
#define FIFO_CTRL_REG 2
#define LINE_CTRL_REG 3
#define LINE_STATUS_REG 5
#define DIVISOR_LATCH_LSB 0
#define DIVISOR_LATCH_MSB 1

#define DIVISOR_LATCH_DIVIDEND  115200 

#define LCR_WORD_LEN (BIT(0) | BIT(1))
#define LCR_NO_STOP_BITS BIT(2)
#define LCR_NO_STOP_PARTIY (BIT(3) | BIT(4) | BIT(5))
#define LCR_SET_BREAK_ENABLE BIT(6)
#define LCR_DLAB_SELECT_DL BIT(7)

#define LSR_RECEIVER_READY BIT(0)
#define LSR_OVERRUN_ERROR BIT(1)
#define LSR_PARITY_ERROR BIT(2)
#define LSR_FRAMING_ERROR BIT(3)
#define LSR_BREAK_INTERRUPT BIT(4)
#define LSR_TRANSMITTER_HOLDING_REGISTER_EMPTY BIT(5)
#define LSR_TRANSMITTER_EMPTY BIT(6)

#define IER_ENABLE_RECEIVED_DATA_INTERRUPT BIT(0)
#define IER_ENABLE_TRANSMITTER_EMPTY_INTERRUPT BIT(1)
#define IER_ENABLE_RECEIVER_LINE_STATUS_INTERRUPT BIT(2)
#define IER_UNCHANGED_BITS (BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7))

#define IIR_INTERRUPT_STATUS BIT(0)
#define IIR_INTERRUPT_ORIGIN (BIT(1) | BIT(2) | BIT(3))
#define IIR_64_BYTE_FIFO BIT(5)
#define IIR_FIFO_STATUS (BIT(6) | BIT(7))

#define FCR_ENABLE_FIFO BIT(0)
#define FCR_CLEAR_RECEIVE_FIFO BIT(1)
#define FCR_CLEAR_TRANSMIT_FIFO BIT(2)
#define FCR_FIFO_INTERRUPT_TRIGGER_LEVEL (BIT(6) | BIT(7))

typedef enum word_len_t {
    WORD_LEN_5_BYTES = 0,
    WORD_LEN_6_BYTES = 1,
    WORD_LEN_7_BYTES = 2,
    WORD_LEN_8_BYTES = 3
} word_len_t;

typedef enum parity_t {
    PARITY_NONE = 0,
    PARITY_ODD = 1,
    PARITY_EVEN = 3,
    PARITY_ONE = 5,
    PARITY_ZERO = 7
} parity_t;

typedef enum no_stop_bits_t {
    STOP_BITS_1 = 0,
    STOP_BITS_2 = 1,
} no_stop_bits_t;

typedef enum fifo_int_trigger_level_t {
    FIFO_1_BYTE = 0,
    FIFO_4_BYTES = 1,
    FIFO_8_BYTES = 2,
    FIFO_14_BYTES = 3
} fifo_int_trigger_level_t;

typedef enum interrupt_origin_t {
    INT_ORIGIN_MODEM_STATUS = 0,
    INT_ORIGIN_TRANSMITTER_EMPTY = 1,
    INT_ORIGIN_CHAR_TIMEOUT = 6,
    INT_ORIGIN_RECEIVED_DATA = 2,
    INT_ORIGIN_LINE_STATUS = 3,
} interrupt_origin_t;

typedef enum fifo_status_t {
    NO_FIFO = 0,
    FIFO_UNUSABLE = 2,
    FIFO_ENABLED = 3,
} fifo_status_t;

typedef struct interrupt_identification_t {
    bool pending;
    interrupt_origin_t origin;
    bool fifo_64_bytes;
    fifo_status_t fifo_status;
} interrupt_identification_t;

int com1_subscribe_int(uint8_t *bit_no);
int com1_unsubscribe_int();
int uart_set_bit_rate(uint16_t bit_rate);
void com1_ih();
int uart_identify_interrupt(interrupt_identification_t *int_ident);
int uart_config_params(word_len_t word_len, parity_t parity, 
                       no_stop_bits_t no_stop_bits, uint16_t bit_rate);
int uart_config_int(bool received_data_int, bool transmitter_empty_int, bool receiver_line_status_int);
int uart_enable_fifo(fifo_int_trigger_level_t trigger_level);
int uart_set_bit_rate(uint16_t bit_rate);
int uart_init_sw_queues();
int uart_receive_bytes();
int uart_send_bytes();
int uart_send_byte(uint8_t byte);
int uart_read_byte(uint8_t *byte);
void uart_flush_RBR();
int uart_clear_hw_fifos();

#endif /* _UART_H */
