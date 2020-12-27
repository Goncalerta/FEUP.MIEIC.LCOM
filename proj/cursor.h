#ifndef _CURSOR_H_
#define _CURSOR_H_

#include <lcom/lcf.h>

#include "mouse.h"

typedef enum cursor_state {
    CURSOR_ARROW,
    CURSOR_PAINT,
    CURSOR_WRITE
} cursor_state;

int cursor_init();
int16_t cursor_get_x();
int16_t cursor_get_y();
void cursor_move(int16_t dx, int16_t dy);
int cursor_draw();
void cursor_set_state(cursor_state state);

#endif /* _CURSOR_H */
