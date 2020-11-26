#ifndef __CANVAS_H
#define __CANVAS_H

#include <lcom/lcf.h>

typedef struct stroke_atom {
    uint16_t x, y; // TODO should atoms be absolute or relative?
} stroke_atom;

typedef struct stroke {
    uint32_t color;
    size_t num_atoms;
    stroke_atom *atoms;
    struct stroke *next, *prev;
} stroke;

int canvas_draw_last_atom();
int clear_canvas();
int canvas_clear_undone();
int canvas_new_stroke(uint32_t color);
int canvas_new_stroke_atom(uint16_t x, uint16_t y);
int canvas_undo_stroke();
int canvas_redo_stroke();
int canvas_draw_strokes();

#endif /* __CANVAS_H */
