#include <lcom/lcf.h>
#include "mouse.h"
#include "kbc.h"
#include "i8042.h"

int hook_id_ms = 2;

void (mouse_update_gesture_state)(uint8_t x_len, uint8_t tolerance, struct packet packet, 
                                  enum mouse_gesture_state *current_state, uint8_t *x_displ) {
    switch (*current_state) {
    case STATE_BEGIN:
        if (packet.lb && !packet.mb && !packet.rb) { 
            *current_state = STATE_LB_PRESSED;
            *x_displ = x_len;
        }
        break;
    case STATE_LB_PRESSED:
        if (!packet.lb || packet.mb || packet.rb || packet.x_ov || packet.y_ov) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (packet.delta_x < -tolerance || packet.delta_y < -tolerance || packet.delta_y/packet.delta_x <= 1) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (*x_displ > packet.delta_x) {
            *x_displ -= packet.delta_x;
        } else {
            *current_state = STATE_FIRST_LINE;
        }
        break;
    case STATE_FIRST_LINE:
        if (packet.mb || packet.rb || packet.x_ov || packet.y_ov) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (packet.delta_x < -tolerance || packet.delta_y < -tolerance || packet.delta_y/packet.delta_x <= 1) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (!packet.lb) {
            *current_state = STATE_LB_RELEASED;
        }
        break;
    case STATE_LB_RELEASED:
        if (packet.lb || packet.mb || packet.x_ov || packet.y_ov) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (packet.delta_x < -tolerance || packet.delta_x > tolerance || packet.delta_y < -tolerance || packet.delta_y > tolerance) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (packet.rb) {
            *current_state = STATE_RB_PRESSED;
            *x_displ = x_len;
        }
        break;
    case STATE_RB_PRESSED:
        if (packet.lb || packet.mb || !packet.rb || packet.x_ov || packet.y_ov) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (packet.delta_x < -tolerance || packet.delta_y > tolerance || packet.delta_y/packet.delta_x >= -1) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (*x_displ > packet.delta_x) {
            *x_displ -= packet.delta_x;
        } else {
            *current_state = STATE_SECOND_LINE;
        }
        break;
    case STATE_SECOND_LINE:
        if (packet.lb || packet.mb || packet.x_ov || packet.y_ov) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (packet.delta_x < -tolerance || packet.delta_y > tolerance || packet.delta_y/packet.delta_x >= -1) {
            *current_state = STATE_BEGIN;
            break;
        }
        if (!packet.rb) {
            *current_state = STATE_RB_RELEASED;
        }
        break;
    case STATE_RB_RELEASED:
        break;
    }
}

void (mouse_ih)() {
    mouse_ih_return = kbc_read_data(&packet_byte, true);
}

int (write_byte_to_mouse)(uint8_t cmd) {
    uint8_t ack;

    do {
        if (kbc_issue_command(CMD_WRITE_BYTE_TO_MS) != OK)
            return 1;
        if (kbc_issue_argument(cmd) != OK)
            return 1;
        if (util_sys_inb(KBC_OUT_BUF, &ack) != OK)
            return 1;
    } while (ack != ACK);

    return 0;
}

int (mouse_enable_dr)() {
    return write_byte_to_mouse(MS_ENABLE_DATA_REPORTING);
}

int (mouse_disable_dr)() {
    return write_byte_to_mouse(MS_DISABLE_DATA_REPORTING);
}

int (mouse_set_stream_mode)() {
    return write_byte_to_mouse(MS_SET_STREAM_MODE);
}

int (mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id_ms;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_ms);
}

int (mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id_ms);
}

bool (mouse_is_valid_first_byte_packet)(uint8_t byte) {
    return (byte & BIT(3));
}

void (mouse_parse_packet)(uint8_t *bytes, struct packet *result) {
    result->bytes[0] = bytes[0];
    result->bytes[1] = bytes[1];
    result->bytes[2] = bytes[2];

    result->rb = bytes[0] & MS_PACKET_RB;
    result->mb = bytes[0] & MS_PACKET_MB;
 	result->lb = bytes[0] & MS_PACKET_LB;
 	result->delta_x = bytes[1]; 
 	result->delta_y = bytes[2];
    if (bytes[0] & MS_PACKET_XDELTA_MSB)
        result->delta_x += 0xff00;
    if (bytes[0] & MS_PACKET_YDELTA_MSB)
        result->delta_y += 0xff00;
 	result->x_ov = bytes[0] & MS_PACKET_XOVFL;
 	result->y_ov = bytes[0] & MS_PACKET_YOVFL;
}
