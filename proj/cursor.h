#ifndef _CURSOR_H_
#define _CURSOR_H_

#include <lcom/lcf.h>

#include "mouse.h"

typedef enum cursor_state {
    CURSOR_ARROW,
    CURSOR_PAINT,
    CURSOR_WRITE,
    CURSOR_DISABLED
} cursor_state;

int cursor_init();
void cursor_exit();
int16_t cursor_get_x();
int16_t cursor_get_y();
int16_t cursor_get_lb();
int16_t cursor_get_rb();
void cursor_move(int16_t dx, int16_t dy);
void cursor_update_buttons(bool new_lb, bool new_rb);
int cursor_draw();
void cursor_set_state(cursor_state state);

#endif /* _CURSOR_H */
