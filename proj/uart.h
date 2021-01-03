#ifndef _UART_H_
#define _UART_H_

#include <lcom/lcf.h>

/** @file 
 * @brief File dedicated to the interaction with the serial port.
 */

/** @defgroup uart uart
 * @{
 *
 * @brief Module dedicated to the interaction with the serial port.
 */

#define COM1_BASE_ADDR 0x3F8 /**< @brief Base adress of COM1 */
#define COM1_IRQ 4 /**< @brief COM1 IRQ line */
#define COM2_BASE_ADDR 0x2F8 /**< @brief Base adress of COM2 */
#define COM2_IRQ 3 /**< @brief COM2 IRQ line */

#define RECEIVER_BUFFER_REG 0 /**< @brief Receiver buffer register */
#define TRANSMITTER_HOLDING_REG 0 /**< @brief Transmitter holding register */
#define INTERRUPT_ENABLE_REG 1 /**< @brief Interrupt enable register */
#define INTERRUPT_IDENTIFICATION_REG 2 /**< @brief Interrupt identification register */
#define FIFO_CTRL_REG 2 /**< @brief FIFO control register */
#define LINE_CTRL_REG 3 /**< @brief Line control register */
#define LINE_STATUS_REG 5 /**< @brief Line status register */
#define DIVISOR_LATCH_LSB 0 /**< @brief Divisor latch least significant byte */
#define DIVISOR_LATCH_MSB 1 /**< @brief Divisor latch most significant byte */

#define DIVISOR_LATCH_DIVIDEND  115200 /**< @brief Constant to be divided by bit rate to obtain the divisor latch value */

#define LCR_WORD_LEN (BIT(0) | BIT(1)) /**< @brief Line control register word lenght mask */
#define LCR_NO_STOP_BITS BIT(2) /**< @brief Line control register number of stop bits mask */
#define LCR_NO_STOP_PARTIY (BIT(3) | BIT(4) | BIT(5)) /**< @brief Line control parity mask */
#define LCR_SET_BREAK_ENABLE BIT(6) /**< @brief Line control register set break enable mask */
#define LCR_DLAB_SELECT_DL BIT(7) /**< @brief Line control register dlab mask */

#define LSR_RECEIVER_READY BIT(0) /**< @brief Line status register receiver ready */
#define LSR_OVERRUN_ERROR BIT(1) /**< @brief Line status register overrun error */
#define LSR_PARITY_ERROR BIT(2) /**< @brief Line status register parity error */
#define LSR_FRAMING_ERROR BIT(3) /**< @brief Line status register framing error */
#define LSR_BREAK_INTERRUPT BIT(4) /**< @brief Line status register break interrupt */
#define LSR_TRANSMITTER_HOLDING_REGISTER_EMPTY BIT(5) /**< @brief Line status register transmitter holding register empty */
#define LSR_TRANSMITTER_EMPTY BIT(6) /**< @brief Line status register transmitter empty */

#define IER_ENABLE_RECEIVED_DATA_INTERRUPT BIT(0) /**< @brief Interrupt enable register enable received data interrupt */
#define IER_ENABLE_TRANSMITTER_EMPTY_INTERRUPT BIT(1) /**< @brief Interrupt enable register enable transmitter empty interrupt */
#define IER_ENABLE_RECEIVER_LINE_STATUS_INTERRUPT BIT(2) /**< @brief Interrupt enable register enable receiver line status interrupt */
#define IER_UNCHANGED_BITS (BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7)) /**< @brief Interrupt enable register unchanged bits mask */

#define IIR_INTERRUPT_STATUS BIT(0) /**< @brief Interrupt identification register interrupt status mask */
#define IIR_INTERRUPT_ORIGIN (BIT(1) | BIT(2) | BIT(3)) /**< @brief Interrupt identification register interrupt origin mask */
#define IIR_64_BYTE_FIFO BIT(5) /**< @brief Interrupt identification register 64 byte FIFO mask */
#define IIR_FIFO_STATUS (BIT(6) | BIT(7)) /**< @brief Interrupt identification register FIFO status mask */

#define FCR_ENABLE_FIFO BIT(0) /**< @brief FIFO control register enable FIFO */
#define FCR_CLEAR_RECEIVE_FIFO BIT(1) /**< @brief FIFO control register clear receive FIFO */
#define FCR_CLEAR_TRANSMIT_FIFO BIT(2) /**< @brief FIFO control register clear transmit FIFO */
#define FCR_FIFO_INTERRUPT_TRIGGER_LEVEL (BIT(6) | BIT(7)) /**< @brief FIFO control register interrupt trigger level mask */

#define UART_DELAY_US 20000 /**< @brief Delay before retrying to read receiver buffer register when flushing it */
#define UART_MAX_TRIES 3 /**< @brief Maximum number of tries to read receiver buffer register when flushing it */

/**
 * @brief Enumerated type for specifying the word lenght parameter of the communication protocol.
 * 
 */
typedef enum word_len {
    WORD_LEN_5_BITS = 0, /*!< Word lenght is 5 bits. */
    WORD_LEN_6_BITS = 1, /*!< Word lenght is 6 bits. */
    WORD_LEN_7_BITS = 2, /*!< Word lenght is 7 bits. */
    WORD_LEN_8_BITS = 3 /*!< Word lenght is 8 bits. */
} word_len_t;

/**
 * @brief Enumerated type for specifying the parity parameter of the communication protocol.
 * 
 */
typedef enum parity {
    PARITY_NONE = 0, /*!< No parity. */
    PARITY_ODD = 1, /*!< Odd parity. */
    PARITY_EVEN = 3, /*!< Even parity. */
    PARITY_ONE = 5, /*!< One parity. */
    PARITY_ZERO = 7 /*!< Zero parity. */
} parity_t;

/**
 * @brief Enumerated type for specifying the number of stop bits parameter of the communication protocol.
 * 
 */
typedef enum no_stop_bits {
    STOP_BITS_1 = 0, /*!< One stop bit. */
    STOP_BITS_2 = 1, /*!< Two stop bits. */
} no_stop_bits_t;

/**
 * @brief Enumerated type for specifying the FIFO interrupt trigger level.
 * 
 */
typedef enum fifo_int_trigger_level {
    FIFO_1_BYTE = 0, /*!< Interrupt when FIFO has 1 byte. */
    FIFO_4_BYTES = 1, /*!< Interrupt when FIFO has 4 bytes. */
    FIFO_8_BYTES = 2, /*!< Interrupt when FIFO has 8 bytes. */
    FIFO_14_BYTES = 3 /*!< Interrupt when FIFO has 14 bytes. */
} fifo_int_trigger_level_t;

/**
 * @brief Enumerated type for representing the origin of the interrupt.
 * 
 */
typedef enum interrupt_origin {
    INT_ORIGIN_MODEM_STATUS = 0, /*!< Interrupt from modem status. */
    INT_ORIGIN_TRANSMITTER_EMPTY = 1, /*!< Interrupt from transmitter empty. */
    INT_ORIGIN_CHAR_TIMEOUT = 6, /*!< Interrupt from character timeout indication. */
    INT_ORIGIN_RECEIVED_DATA = 2, /*!< Interrupt from received data available. */
    INT_ORIGIN_LINE_STATUS = 3, /*!< Interrupt from line status. */
} interrupt_origin_t;

/**
 * @brief Enumerated type for representing the FIFO status.
 * 
 */
typedef enum fifo_status {
    NO_FIFO = 0, /*!< No FIFO enabled. */
    FIFO_UNUSABLE = 2, /*!< FIFO is unusable. */
    FIFO_ENABLED = 3, /*!< FIFO enabled. */
} fifo_status_t;

/**
 * @brief Interrupt identification info.
 * 
 */
typedef struct interrupt_identification {
    bool pending; /*!< @brief Whether an interruption is pending. */
    interrupt_origin_t origin; /*!< @brief The origin of the interrupt. */
    bool fifo_64_bytes; /*!< @brief True if 64-byte FIFO, false otherwise. */
    fifo_status_t fifo_status; /*!< @brief FIFO status. */
} interrupt_identification_t;

/**
 * @brief Subscribes COM1 interrupts.
 * 
 * @param bit_no address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int com1_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes COM1 interrupts.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int com1_unsubscribe_int();

/**
 * @brief COM1 interrupt handler.
 * 
 */
void com1_ih();

/**
 * @brief Handles a serial port error interrupt.
 * 
 */
void uart_handle_error();

/**
 * @brief Indentifies the serial port interrupt.
 * 
 * @param int_ident the variable to be filled with the interrupt information
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_identify_interrupt(interrupt_identification_t *int_ident);

/**
 * @brief Configures the parameters of the serial port.
 * 
 * @param word_len number of bits in a word
 * @param parity type of parity check
 * @param no_stop_bits number of stop bits
 * @param bit_rate the bit rate of the communication
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_config_params(word_len_t word_len, parity_t parity, 
                       no_stop_bits_t no_stop_bits, uint16_t bit_rate);

/**
 * @brief Configures the interrupts of the serial port.
 * 
 * @param received_data_int whether received data interrupt should be enabled
 * @param transmitter_empty_int whether transmitter empty interrupt should be enabled
 * @param receiver_line_status_int whether receiver line status interrupt should be enabled
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_config_int(bool received_data_int, bool transmitter_empty_int, bool receiver_line_status_int);

/**
 * @brief Enables serial port hardware FIFOs.
 * 
 * @param trigger_level number of bytes in FIFO in order to trigger interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_enable_fifo(fifo_int_trigger_level_t trigger_level);

/**
 * @brief Disables serial port hardware FIFOs.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_disable_fifo();

/**
 * @brief Specifies the bit rate for the serial port.
 * 
 * @param bit_rate the bit rate of the communication
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_set_bit_rate(uint16_t bit_rate);

/**
 * @brief Initializes serial port software queues.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_init_sw_queues();

/**
 * @brief Frees the memory allocated for serial port software queues.
 * 
 */
void uart_delete_sw_queues();

/**
 * @brief Receives bytes from the serial port and puts them in a software queue.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_receive_bytes();

/**
 * @brief Sends bytes from a software queue into the serial port.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_send_bytes();

/**
 * @brief Adds a byte to the software queue of bytes to be sent and tries to send bytes.
 * 
 * @param byte byte to be sent
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_send_byte(uint8_t byte);

/**
 * @brief Reads a byte from the software queue of bytes received.
 * 
 * @param byte address of memory to be set with the byte read
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_read_byte(uint8_t *byte);

/**
 * @brief Checks if there are unread received bytes in the software queue.
 * 
 * @return Return whether there are unread received bytes in the software queue
 */
bool uart_received_bytes();

/**
 * @brief Flushes all bytes being received from the serial port.
 * 
 * The receiver buffer register is read multiple times after some delay to make 
 * sure there aren't still bytes of the same message being sent.
 * 
 * The first and last bytes, as well as the number of bytes cleared are returned
 * in order to allow checking for an acknowledgment byte sent before or after the message.
 * 
 * @param no_bytes address of memory to be set with number of bytes being cleared
 * @param first address of memory to be set with the first byte being cleared
 * @param last address of memory to be set with the last byte being cleared
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_flush_received_bytes(uint8_t *no_bytes, uint8_t *first, uint8_t *last);

/**
 * @brief Clears both serial port hardware FIFOs.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int uart_clear_hw_fifos();

/**@}*/

#endif /* _UART_H */
