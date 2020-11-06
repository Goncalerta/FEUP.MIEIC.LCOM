#ifndef __MOUSE_H
#define __MOUSE_H

uint8_t packet_byte;

int (write_byte_to_mouse)(uint8_t cmd);

int (mouse_enable_dr)();

int (mouse_disable_dr)();

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

bool (mouse_is_valid_first_byte_packet)(uint8_t byte);

void (mouse_parse_packet)(uint8_t *bytes, struct packet *result);

#endif /* __MOUSE_H */
