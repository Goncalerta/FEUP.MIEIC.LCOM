#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#include <lcom/lcf.h>
#include "keyboard.h"

int dispatch_mouse_packet(struct packet p);
int dispatch_keyboard_event(KBD_STATE s);
int dispatch_timer_tick(unsigned int interrupt_counter);
int draw_frame();
bool should_end();

#endif /* __DISPATCHER_H */
