#include <lcom/lcf.h>

#include "uart.h"

static int hook_id_com1 = 4;

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
    
    // TODO check fifo status: where? after configuring fifo or in int handler?

    switch (int_ident.origin) {
    case INT_ORIGIN_TRANSMITTER_EMPTY:
        break;
    case INT_ORIGIN_CHAR_TIMEOUT:
        break;
    case INT_ORIGIN_RECEIVED_DATA:
        break;
    case INT_ORIGIN_LINE_STATUS:
        break;
    default:
        break;
    }
}

int uart_send_byte(uint8_t byte) {

}

int uart_receive_byte(uint8_t *byte) {

}

void uart_flush_RBR() {
    uint8_t lsr_byte, rbr_byte;

    if (util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REG, &lsr_byte) != OK)
        return;

    if (lsr_byte & LSR_RECEIVER_READY) {
        if (util_sys_inb(COM1_BASE_ADDR + REGISTER_BUFFER_REG, &rbr_byte) != OK)
            return;
    }
}

int uart_identify_interrupt(interrupt_identification_t *int_ident) {
    uint8_t iir_byte;
    if (util_sys_inb(COM1_BASE_ADDR + INTERRUPT_IDENTIFICATION_REG, &iir_byte) != OK)
        return 1;
    
    int_ident->pending = iir_byte & IIR_INTERRUPT_STATUS;
    int_ident->origin = (iir_byte & IIR_INTERRUPT_ORIGIN) >> 1;
    int_ident->fifo_64_bytes = (iir_byte & IIR_64_BYTE_FIFO) >> 5;
    int_ident->fifo_status = (iir_byte & IIR_FIFO_STATUS) >> 6;

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
    ier_byte |= (received_data_int << IER_ENABLE_RECEIVED_DATA_INTERRUPT)
              | (transmitter_empty_int << IER_ENABLE_TRANSMITTER_EMPTY_INTERRUPT)
              | (receiver_line_status_int << IER_ENABLE_RECEIVER_LINE_STATUS_INTERRUPT);

    if (sys_outb(COM1_BASE_ADDR + INTERRUPT_ENABLE_REG, ier_byte) != OK)
        return 1;
    
    return 0;
}

int uart_enable_fifo(fifo_int_trigger_level_t trigger_level) {
    uint8_t fcr_byte = FCR_ENABLE_FIFO | (trigger_level << 6);
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
