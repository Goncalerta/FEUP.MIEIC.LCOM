#ifndef __MOUSE_H
#define __MOUSE_H

uint8_t packet_byte, mouse_ih_return;

enum mouse_gesture_state {
    STATE_BEGIN,
    STATE_LB_PRESSED,
    STATE_FIRST_LINE,
    STATE_LB_RELEASED,
    STATE_RB_PRESSED,
    STATE_SECOND_LINE,
    STATE_RB_RELEASED,
};

void (mouse_update_gesture_state)(uint8_t x_len, uint8_t tolerance, struct packet packet, 
                                  enum mouse_gesture_state *current_state, uint8_t *x_displ);

int (write_byte_to_mouse)(uint8_t cmd);

int (mouse_enable_dr)();

int (mouse_disable_dr)();

int (mouse_set_stream_mode)();

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

bool (mouse_is_valid_first_byte_packet)(uint8_t byte);

void (mouse_parse_packet)(uint8_t *bytes, struct packet *result);

#endif /* __MOUSE_H */
