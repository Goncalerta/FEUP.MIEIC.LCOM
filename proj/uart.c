#include <lcom/lcf.h>

#include "uart.h"
#include "queue.h"

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
    
    while (true) {
        if (uart_identify_interrupt(&int_ident) != OK)  {
            printf("Error identifying uart interrupt\n");
            return;
        }
        
        if (!int_ident.pending)
            return;

        switch (int_ident.origin) {
        case INT_ORIGIN_TRANSMITTER_EMPTY:
            if (uart_send_bytes() != OK) {
                printf("Error sending uart bytes\n");
            }
            break;

        case INT_ORIGIN_CHAR_TIMEOUT:
        case INT_ORIGIN_RECEIVED_DATA:
            if (uart_receive_bytes() != OK) {
                printf("Error receiving uart bytes\n");
            }
            break;
        
        case INT_ORIGIN_LINE_STATUS:
            uart_handle_error();
            break;
        
        default:
            break;
        }
    }
}

bool uart_error_reading_message() {
    return error_reading_message;
}

int uart_send_byte(uint8_t byte) {
    if (queue_push(&transmitted, &byte) != OK)
        return 1;
    
    return uart_send_bytes();
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

void uart_handle_error() {
    bool err;
    if (uart_check_error(&err) != OK) {
        printf("Failed to handle uart error.\n");
        return;
    }
    
    if (err) {
        error_reading_message = true;
    }
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

void uart_delete_sw_queues() {
    delete_queue(&transmitted);
    delete_queue(&received);
}

int uart_flush_received_bytes(uint8_t *no_bytes, uint8_t *first, uint8_t *last) {
    *no_bytes = 0;

    // First empty received bytes in queue
    while (!queue_is_empty(&received)) {
        if (queue_top(&received, last) != OK)
            return 1;
        if (no_bytes == 0) {
            *first = *last;
        }
        *no_bytes += 1;
        if (queue_pop(&received) != OK)
            return 1;
    }

    // Now read the receiver buffer to make sure no new bytes were received
    // Wait and try 3 times to make sure that the message being received was completely received
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
            *no_bytes += 1;
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

int uart_config_params(word_len_t word_len, parity_t parity, 
                       no_stop_bits_t no_stop_bits, uint16_t bit_rate) {
    uint8_t lcr_byte = word_len | (parity << 3) | (no_stop_bits << 2);

    if (sys_outb(COM1_BASE_ADDR + LINE_CTRL_REG, lcr_byte) != OK)
        return 1;
    
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

static int uart_select_dlab() {
    uint8_t lcr_byte;
    if (util_sys_inb(COM1_BASE_ADDR + LINE_CTRL_REG, &lcr_byte) != OK)
        return 1;

    lcr_byte |= LCR_DLAB_SELECT_DL;
    if (sys_outb(COM1_BASE_ADDR + LINE_CTRL_REG, lcr_byte) != OK)
        return 1;
    
    return 0;
}

static int uart_select_data() {
    uint8_t lcr_byte;
    if (util_sys_inb(COM1_BASE_ADDR + LINE_CTRL_REG, &lcr_byte) != OK)
        return 1;

    lcr_byte &= ~LCR_DLAB_SELECT_DL;
    if (sys_outb(COM1_BASE_ADDR + LINE_CTRL_REG, lcr_byte) != OK)
        return 1;
    
    return 0;
}

int uart_set_bit_rate(uint16_t bit_rate) {
    if (uart_select_dlab() != OK)
        return 1;

    uint16_t divisor_latch_value = DIVISOR_LATCH_DIVIDEND / bit_rate;
    uint8_t divisor_latch_lsb, divisor_latch_msb;
    
    if (util_get_LSB(divisor_latch_value, &divisor_latch_lsb) != OK) {
        uart_select_data();
        return 1;
    }
        
    if (util_get_MSB(divisor_latch_value, &divisor_latch_msb) != OK) {
        uart_select_data();
        return 1;
    }

    if (sys_outb(COM1_BASE_ADDR + DIVISOR_LATCH_LSB, divisor_latch_lsb) != OK) {
        uart_select_data();
        return 1;
    }
    
    if (sys_outb(COM1_BASE_ADDR + DIVISOR_LATCH_MSB, divisor_latch_msb) != OK) {
        uart_select_data();
        return 1;
    }

    if (uart_select_data() != OK)
        return 1;
    
    return 0;
}
