#ifndef __CANVAS_H
#define __CANVAS_H

#include <lcom/lcf.h>

typedef enum canvas_state_t {
    CANVAS_STATE_DISABLED, // TODO continue from here
    CANVAS_STATE_NORMAL,
    CANVAS_STATE_HOVERING,
    CANVAS_STATE_PRESSING_LB,
    CANVAS_STATE_PRESSING_RB,
} canvas_state_t;

typedef struct stroke_atom {
    uint16_t x, y;
} stroke_atom;

typedef struct stroke {
    uint32_t color;
    uint16_t thickness;
    size_t num_atoms;
    stroke_atom *atoms;
    struct stroke *next, *prev;
} stroke;

int canvas_init(uint16_t width, uint16_t height);
int canvas_exit();
int canvas_draw_frame(uint16_t y);
int clear_canvas();
int canvas_new_stroke(uint32_t color, uint16_t thickness);
int canvas_new_stroke_atom(uint16_t x, uint16_t y);
int canvas_undo_stroke();
int canvas_redo_stroke();
bool canvas_is_hovering(uint16_t x, uint16_t y);
int canvas_update_state(bool hovering, bool lb, bool rb);
canvas_state_t canvas_get_state();

#endif /* __CANVAS_H */
