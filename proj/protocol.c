#include <lcom/lcf.h>

#include "uart.h"
#include "protocol.h"

int protocol_config_uart() {
    if (uart_init_sw_queues() != OK)
        return 1;
    if (uart_config_params(WORD_LEN_8_BYTES, PARITY_ODD, STOP_BITS_2, PROTOCOL_BIT_RATE) != OK)
        return 1;
    if (uart_config_int(true, true, true) != OK)
        return 1;
    if (uart_enable_fifo(FIFO_8_BYTES) != OK)
        return 1;
    
    return 0;
}
