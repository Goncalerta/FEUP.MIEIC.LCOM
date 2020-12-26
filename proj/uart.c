#include <lcom/lcf.h>

#include "uart.h"
#include "queue.h"
#include "utilities.h"

#define UART_SW_QUEUES_STARTING_CAPACITY 16

static int hook_id_com1 = 4;

static queue_t transmitted, received;
static fifo_int_trigger_level_t fifo_int_trigger_level;
static bool error_reading_message = false;

int com1_subscribe_int(uint8_t *bit_no) {
    *bit_no = hook_id_com1;
    return sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_com1);
}

int com1_unsubscribe_int() {
    return sys_irqrmpolicy(&hook_id_com1);
}

void com1_ih() {
    interrupt_identification_t int_ident;
    
    if (uart_identify_interrupt(&int_ident) != OK) 
        return;
    
    if (!int_ident.pending)
        return;

    switch (int_ident.origin) {
    case INT_ORIGIN_TRANSMITTER_EMPTY:
        uart_send_bytes();
        break;

    case INT_ORIGIN_CHAR_TIMEOUT:
    case INT_ORIGIN_RECEIVED_DATA:
        uart_receive_bytes();
        break;
    
    case INT_ORIGIN_LINE_STATUS:
        uart_handle_error();
        break;
    
    default:
        break;
    }
}

bool uart_error_reading_message() {
    return error_reading_message;
}

// int uart_send_message(uint8_t len, uint8_t *msg) {
//     if (len == 0)
//         return 1;

//     if (uart_send_byte(len + 1) != OK)
//         return 1;
    
//     for (uint8_t i = 0; i < len; i++) {
//         if (uart_send_byte(msg[i]) != OK)
//             return 1;
//     }

//     awaiting_ack = true;

//     return 0;
// }

// int uart_receive_message(uint8_t **msg) {
//     if (len == 0)
//         return 1;
    

// }

int uart_send_byte(uint8_t byte) {
    if (queue_push(&transmitted, &byte) != OK)
        return 1;
    
    return uart_send_bytes(); // TODO maybe don't flush automatically each byte; instead flush each message
}

int uart_read_byte(uint8_t *byte) {
    if (queue_is_empty(&received) != OK)
        return 1;
    if (queue_top(&received, byte) != OK)
        return 1;
    if (queue_pop(&received) != OK)
        return 1;
    return 0;
}

bool uart_received_bytes() {
    return !queue_is_empty(&received);
}

int uart_receive_bytes() {
    uint8_t lsr_byte, rbr_byte;
    
    if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
        return 1;
    
    while (lsr_byte & LSR_RECEIVER_READY) {
        if (util_sys_inb(COM1_BASE_ADDR + RECEIVER_BUFFER_REG, &rbr_byte) != OK)
            return 1;
        if (queue_push(&received, &rbr_byte) != OK)
            return 1;
        if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
            return 1;
    }

    return 0;
}

int uart_send_bytes() {
    uint8_t lsr_byte, thr_byte;
    
    if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
        return 1;
    
    while (!queue_is_empty(&transmitted) && (lsr_byte & LSR_TRANSMITTER_HOLDING_REGISTER_EMPTY)) {
        if (queue_top(&transmitted, &thr_byte) != OK)
            return 1;
        if (queue_pop(&transmitted) != OK)
            return 1;
        if (sys_outb(COM1_BASE_ADDR + TRANSMITTER_HOLDING_REG, thr_byte) != OK)
            return 1;
        if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
            return 1;
    }

    return 0;
}

// int uart_send_byte(uint8_t byte) {
//     uint8_t lsr_byte;
    
//     if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
//         return 1;
    
//     while (!(lsr_byte & LSR_TRANSMITTER_HOLDING_REGISTER_EMPTY)) {
//         tickdelay(micros_to_ticks(DELAY_US));
//         if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
//             return 1;
//     }
    
//     if (sys_outb(COM1_BASE_ADDR + TRANSMITTER_HOLDING_REG, byte) != OK)
//         return 1;

//     return 0;
// }

// int uart_receive_byte(uint8_t *byte) {
//     uint8_t lsr_byte;
    
//     if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
//         return 1;
    
//     while (!(lsr_byte & LSR_RECEIVER_READY)) {
//         tickdelay(micros_to_ticks(DELAY_US));
//         if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
//             return 1;
//     }
    
//     if (util_sys_inb(COM1_BASE_ADDR + RECEIVER_BUFFER_REG, byte) != OK)
//         return 1;

//     return 0;
// }
void uart_handle_error() {
    bool err;
    if (uart_check_error(&err) != OK)
        return;
    
    if (uart_check_error) {
        error_reading_message = true;
    }
}

int uart_check_error(bool *err) {
    uint8_t lsr_byte;
    
    if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
        return 1;
        
    if (lsr_byte & (LSR_OVERRUN_ERROR | LSR_PARITY_ERROR | LSR_FRAMING_ERROR)) {
        *err = true;
    } else {
        *err = false;
    }

    return 0;
}

int uart_init_sw_queues() {
    if (new_queue(&transmitted, sizeof(uint8_t), UART_SW_QUEUES_STARTING_CAPACITY) != OK) {
        return 1;
    }

    if (new_queue(&received, sizeof(uint8_t), UART_SW_QUEUES_STARTING_CAPACITY) != OK) {
        return 1;
    }

    return 0;
}

int uart_flush_received_bytes(uint8_t *no_bytes, uint8_t *first, uint8_t *last) {
    *no_bytes = 0;
    while (!queue_is_empty(&received)) {
        if (queue_top(&received, last) != OK)
            return 1;
        if (no_bytes == 0) {
            *first = *last;
        }
        *no_bytes++;
        if (queue_pop(&received) != OK)
            return 1;
    }

    uint8_t lsr_byte, rbr_byte;
    
    if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
        return 1;
    
    uint8_t tries = 0;
    while (tries < 3) {
        while (lsr_byte & LSR_RECEIVER_READY) {
            tries = 0;
            if (util_sys_inb(COM1_BASE_ADDR + RECEIVER_BUFFER_REG, &rbr_byte) != OK)
                return 1;
            *last = rbr_byte;
            if (no_bytes == 0) {
                *first = *last;
            }
            *no_bytes++;
            if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
                return 1;
        }
        tries++;
        if (tries < UART_MAX_TRIES) {
            // Wait some time to make sure part of the message to be ignored
            // isn't still being sent
            tickdelay(micros_to_ticks(UART_DELAY_US));
        }
    }

    error_reading_message = false;

    return 0;
}

void uart_flush_RBR() {
    uint8_t lsr_byte, rbr_byte;

    if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
        return;

    if (lsr_byte & LSR_RECEIVER_READY) {
        if (util_sys_inb(COM1_BASE_ADDR + RECEIVER_BUFFER_REG, &rbr_byte) != OK)
            return;
    }
}

int uart_identify_interrupt(interrupt_identification_t *int_ident) {
    uint8_t iir_byte;
    if (util_sys_inb(COM1_BASE_ADDR + INTERRUPT_IDENTIFICATION_REG, &iir_byte) != OK)
        return 1;
    
    int_ident->pending = !(iir_byte & IIR_INTERRUPT_STATUS);
    int_ident->origin = (iir_byte & IIR_INTERRUPT_ORIGIN) >> 1;
    int_ident->fifo_64_bytes = (iir_byte & IIR_64_BYTE_FIFO) >> 5;
    int_ident->fifo_status = (iir_byte & IIR_FIFO_STATUS) >> 6;

    return 0;
}

int uart_clear_hw_fifos() {
    uint8_t fcr_byte = FCR_ENABLE_FIFO | (fifo_int_trigger_level << 6) 
                     | FCR_CLEAR_RECEIVE_FIFO | FCR_CLEAR_TRANSMIT_FIFO;
    uint8_t iir_byte;

    if (sys_outb(COM1_BASE_ADDR + FIFO_CTRL_REG, fcr_byte) != OK)
        return 1;
    
    if (util_sys_inb(COM1_BASE_ADDR + INTERRUPT_IDENTIFICATION_REG, &iir_byte) != OK)
        return 1;
    
    fifo_status_t fifo_status = (iir_byte & IIR_FIFO_STATUS) >> 6;
    if (fifo_status != FIFO_ENABLED)
        return 1;

    return 0;
}

// static int uart_read_reg(uint8_t reg, uint8_t *data) {
//     if (util_sys_inb(COM1_BASE_ADDR + reg, data) != OK)
//         return 1;
    
//     return 0;
// }

// static int uart_write_reg(uint8_t reg, uint8_t data) {
//     if (sys_outb(COM1_BASE_ADDR + reg, data) != OK)
//         return 1;
//     return 0;
// }

int uart_config_params(word_len_t word_len, parity_t parity, 
                       no_stop_bits_t no_stop_bits, uint16_t bit_rate) {
    // TODO should we care about preserving Set Break Enable
    uint8_t lcr_byte = word_len | (parity << 3) | (no_stop_bits << 2);

    if (sys_outb(COM1_BASE_ADDR + LINE_CTRL_REG, lcr_byte) != OK)
        return 1;
    
    // TODO maybe try to avoid this because of the unecessary sys_inb? but then it wouldnt be modular
    if (uart_set_bit_rate(bit_rate) != OK)
        return 1;
    
    return 0;
}

int uart_config_int(bool received_data_int, bool transmitter_empty_int, bool receiver_line_status_int) {
    uint8_t ier_byte;
    if (util_sys_inb(COM1_BASE_ADDR + INTERRUPT_ENABLE_REG, &ier_byte) != OK)
        return 1;

    ier_byte &= IER_UNCHANGED_BITS;
    ier_byte |= (received_data_int << 0)
              | (transmitter_empty_int << 1)
              | (receiver_line_status_int << 2);

    if (sys_outb(COM1_BASE_ADDR + INTERRUPT_ENABLE_REG, ier_byte) != OK)
        return 1;
    
    return 0;
}

int uart_enable_fifo(fifo_int_trigger_level_t trigger_level) {
    fifo_int_trigger_level = trigger_level;
    uint8_t fcr_byte = FCR_ENABLE_FIFO | (trigger_level << 6);
    uint8_t iir_byte;

    if (sys_outb(COM1_BASE_ADDR + FIFO_CTRL_REG, fcr_byte) != OK)
        return 1;
    
    if (util_sys_inb(COM1_BASE_ADDR + INTERRUPT_IDENTIFICATION_REG, &iir_byte) != OK)
        return 1;
    
    fifo_status_t fifo_status = (iir_byte & IIR_FIFO_STATUS) >> 6;
    if (fifo_status != FIFO_ENABLED)
        return 1;

    return 0;
}

int uart_disable_fifo() {
    uint8_t fcr_byte = 0;
    if (sys_outb(COM1_BASE_ADDR + FIFO_CTRL_REG, fcr_byte) != OK)
        return 1;
    return 0;
}

int uart_set_bit_rate(uint16_t bit_rate) {
    uint8_t lcr_byte;
    if (util_sys_inb(COM1_BASE_ADDR + LINE_CTRL_REG, &lcr_byte) != OK)
        return 1;

    lcr_byte |= LCR_DLAB_SELECT_DL;
    if (sys_outb(COM1_BASE_ADDR + LINE_CTRL_REG, lcr_byte) != OK)
        return 1;

    uint16_t divisor_latch_value = DIVISOR_LATCH_DIVIDEND / bit_rate;
    uint8_t divisor_latch_lsb, divisor_latch_msb;
    // TODO maybe instead of return 1 first revert change to DLAB
    if (util_get_LSB(divisor_latch_value, &divisor_latch_lsb) != OK)
        return 1;
    if (util_get_MSB(divisor_latch_value, &divisor_latch_msb) != OK)
        return 1;

    if (sys_outb(COM1_BASE_ADDR + DIVISOR_LATCH_LSB, divisor_latch_lsb) != OK)
        return 1;
    
    if (sys_outb(COM1_BASE_ADDR + DIVISOR_LATCH_MSB, divisor_latch_msb) != OK)
        return 1;

    lcr_byte &= ~LCR_DLAB_SELECT_DL;
    if (sys_outb(COM1_BASE_ADDR + LINE_CTRL_REG, lcr_byte) != OK)
        return 1;
    return 0;
}
