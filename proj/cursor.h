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
void update_cursor(struct packet *p);
void cursor_draw();

#endif /* _CURSOR_H */
