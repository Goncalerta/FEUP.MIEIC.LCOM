#include <lcom/lcf.h>
#include "mouse.h"
#include "kbc.h"
#include "i8042.h"

int hook_id_ms;

void (mouse_ih)() {
  ih_return = kbc_read_data(&packet_byte, 1);
}

int (write_byte_to_mouse)(uint8_t cmd) {
    uint8_t ack;

    do {
        if (kbc_issue_cmd(CMD_WRITE_BYTE_TO_MS))
            return 1;
        if (kbc_pass_cmd_arg(cmd))
            return 1;
        if (util_sys_inb(KBC_OUT_BUF, &ack))
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

int (mouse_subscribe_int)(uint8_t *bit_no) {
  hook_id_ms = *bit_no;
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
 	result->delta_x = (uint16_t) bytes[1]; 
 	result->delta_y = (uint16_t) bytes[2];
    if (bytes[0] & MS_PACKET_XDELTA_MSB)
        result->delta_x += 0xff00;
    if (bytes[0] & MS_PACKET_YDELTA_MSB)
        result->delta_y += 0xff00;
 	result->x_ov = bytes[0] & MS_PACKET_XOVFL;
 	result->y_ov = bytes[0] & MS_PACKET_YOVFL;
}
