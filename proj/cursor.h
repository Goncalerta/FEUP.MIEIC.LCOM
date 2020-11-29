#ifndef _CURSOR_H_
#define _CURSOR_H_

#include <lcom/lcf.h>

#include "mouse.h"

typedef enum cursor_state {
    CURSOR_ARROW,
    CURSOR_PAINT,
    CURSOR_WRITE
} cursor_state;

void cursor_init();
int16_t cursor_get_x();
int16_t cursor_get_y();
bool cursor_is_lb_pressed();
bool cursor_is_rb_pressed();
void cursor_move(int16_t dx, int16_t dy);
bool cursor_set_lb_state(bool pressed);
bool cursor_set_rb_state(bool pressed);
int cursor_draw();

#endif /* _CURSOR_H */
