#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#include <lcom/lcf.h>
#include "keyboard.h"

int dispatch_mouse_packet(struct packet p);
int dispatch_keyboard_event(KBD_STATE s);
int dispatch_timer_tick();
int draw_frame();
int event_new_stroke(bool primary_button);
int event_new_atom(uint16_t x, uint16_t y);
bool should_end();

#endif /* __DISPATCHER_H */