#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#include <lcom/lcf.h>
#include "keyboard.h"
#include "textbox.h"
#include "protocol.h"

int dispatcher_bind_buttons(size_t number_of_buttons, ...);
int dispatcher_bind_text_boxes(size_t number_of_text_boxes, ...);
int dispatcher_bind_canvas(bool is_to_bind);

int dispatch_mouse_packet(struct packet p);
int dispatch_keyboard_event(kbd_event_t s);
int dispatch_timer_tick();
int dispatch_rtc_alarm_int();
int dispatch_rtc_periodic_int();

int draw_frame();
int event_update_cursor_state();
int event_new_stroke(bool primary_button);
int event_new_atom(uint16_t x, uint16_t y);
int event_end_program();
int event_round_win(uint32_t score);
int event_end_round();
int event_undo();
int event_redo();
int event_ready_to_play();
int event_other_player_ready_to_play();
int event_new_round_as_guesser(const char *word);
int event_new_round_as_drawer();
int event_start_round();
int event_leave_game();
int event_other_player_leave_game();
int event_this_player_random_number();
int event_other_player_random_number();
int event_other_player_opened_program();

bool should_end();

#endif /* __DISPATCHER_H */
