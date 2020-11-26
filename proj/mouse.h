#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lcf.h>

int mouse_ih_return;

int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

bool mouse_packet_ready();

int mouse_retrieve_packet(struct packet *packet);

int write_byte_to_mouse(uint8_t cmd);

int mouse_enable_dr();

int mouse_disable_dr();

int mouse_set_stream_mode();

#endif /* __MOUSE_H */
