#include <lcom/lcf.h>
#include "mouse.h"
#include "kbc.h"
#include "i8042.h"

static int hook_id_mouse = 2;
static uint8_t raw_packet[3];
static size_t packet_byte_counter = 0;

static bool mouse_is_valid_first_byte_packet(uint8_t byte) {
    return (byte & BIT(3));
}

int mouse_subscribe_int(uint8_t *bit_no) {
  *bit_no = hook_id_mouse;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse);
}

int mouse_unsubscribe_int() {
  return sys_irqrmpolicy(&hook_id_mouse);
}

void (mouse_ih)() {
    if (packet_byte_counter >= 3) {
        mouse_ih_return = 1;
        return;
    }
    
    mouse_ih_return = kbc_read_data(&raw_packet[packet_byte_counter++]);

    if (mouse_ih_return == OK && packet_byte_counter == 1) {
        if (!mouse_is_valid_first_byte_packet(raw_packet[0])) {
            mouse_ih_return = 1;
            packet_byte_counter = 0;
        }
    }
}

bool mouse_packet_ready() {
    return packet_byte_counter == 3;
}

int mouse_retrieve_packet(struct packet *packet) {
    if (packet_byte_counter <= 2) 
        return 1;
    
    packet->bytes[0] = raw_packet[0];
    packet->bytes[1] = raw_packet[1];
    packet->bytes[2] = raw_packet[2];

    packet->rb = raw_packet[0] & MS_PACKET_RB;
    packet->mb = raw_packet[0] & MS_PACKET_MB;
 	packet->lb = raw_packet[0] & MS_PACKET_LB;
 	packet->delta_x = raw_packet[1]; 
 	packet->delta_y = raw_packet[2];
    if (raw_packet[0] & MS_PACKET_XDELTA_MSB)
        packet->delta_x += 0xff00;
    if (raw_packet[0] & MS_PACKET_YDELTA_MSB)
        packet->delta_y += 0xff00;
 	packet->x_ov = raw_packet[0] & MS_PACKET_XOVFL;
 	packet->y_ov = raw_packet[0] & MS_PACKET_YOVFL;

    packet_byte_counter = 0;
    return 0;
}

int write_byte_to_mouse(uint8_t cmd) {
    uint8_t ack;

    do {
        if (kbc_issue_command(CMD_WRITE_BYTE_TO_MS) != OK)
            return 1;
        if (kbc_issue_argument(cmd) != OK)
            return 1;
        if (util_sys_inb(KBC_OUT_BUF, &ack) != OK)
            return 1;
    } while (ack != ACK); //TODO não tem que se fazer tickdelays entre iterações?

    return 0;
}

int mouse_enable_dr() {
    return write_byte_to_mouse(MS_ENABLE_DATA_REPORTING);
}

int mouse_disable_dr() {
    return write_byte_to_mouse(MS_DISABLE_DATA_REPORTING);
}

int mouse_set_stream_mode() {
    return write_byte_to_mouse(MS_SET_STREAM_MODE);
}
