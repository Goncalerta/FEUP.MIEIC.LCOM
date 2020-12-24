#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <lcom/lcf.h>

/* TODO Which one should we use? 1200, 2400, 4800, 9600, 19200, 38400, 57600 or 115200 */
#define PROTOCOL_BIT_RATE 9600

int protocol_config_uart();

#endif /* _PROTOCOL_H */
