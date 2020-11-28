#include <lcom/lcf.h>
#include <math.h>
#include "canvas.h"
#include "video_gr.h"
#include "graphics.h"

#define CANVAS_WIDTH 5
static stroke *first, *last, *undone;
static video_buffer_t canvas_buf; // current picture drawn in buffer - copied into vcard back buffer

int canvas_init(uint16_t width, uint16_t height) {
    canvas_buf.h_res = width;
    canvas_buf.v_res = height;
    canvas_buf.bytes_per_pixel = vg_get_bytes_per_pixel();
    canvas_buf.buf = malloc(sizeof(uint8_t) * width * height * canvas_buf.bytes_per_pixel);
    
    vb_fill_screen(canvas_buf, 0x00ffffff);

    first = NULL;
    last = NULL;
    undone = NULL;

    return 0;
}

static int canvas_draw_atom_line(stroke_atom atom1, stroke_atom atom2, uint32_t color) {
    uint16_t x1 = atom1.x;
    uint16_t y1 = atom1.y;
    uint16_t x2 = atom2.x;
    uint16_t y2 = atom2.y;

    if (vb_draw_line(canvas_buf, x1, y1, x2, y2, color, CANVAS_WIDTH) != OK)
        return 1;
    
    return 0;
}

static int canvas_draw_last_atom() {
    if (last->num_atoms == 1) {
        stroke_atom atom = last->atoms[0];
        return vb_draw_circle(canvas_buf, atom.x, atom.y, CANVAS_WIDTH, last->color);
    } else {
        return canvas_draw_atom_line(last->atoms[last->num_atoms-2], last->atoms[last->num_atoms-1], last->color);
    }
}

static int canvas_draw_stroke(stroke *stroke) {
    for (size_t i = 1; i < stroke->num_atoms; i++) {
        if (canvas_draw_atom_line(stroke->atoms[i-1], stroke->atoms[i], stroke->color) != OK)
            return 1;
    }

    return 0;
}

static int canvas_redraw_strokes() {
    vb_fill_screen(canvas_buf, 0x00ffffff);

    stroke *current = first;
    while (current != NULL) {
        canvas_draw_stroke(current);

        current = current->next;
    }

    return 0;
}

int clear_canvas() {
    stroke *current = last;
    while (current != NULL) {
        stroke *prev = current->prev;
        free(current);
        current = prev;
    }
    
    first = NULL;
    last = NULL;

    canvas_clear_undone();
    return 0;
}

int canvas_clear_undone() {
    stroke *current = undone;
    while (current != NULL) {
        stroke *prev = current->prev;
        free(current);
        current = prev;
    }
    
    undone = NULL;
    return 0;
}

int canvas_exit() {
    if (clear_canvas() != OK)
        return 1;
    free(canvas_buf.buf);
    return 0;
}

int canvas_new_stroke(uint32_t color) {
    stroke *s = malloc(sizeof(stroke));
    s->atoms = NULL;
    s->color = color;
    s->next = NULL;
    s->prev = NULL;
    s->num_atoms = 0;

    if (first == NULL) {
        first = s;
        last = s;
    } else {
        s->prev = last;
        last->next = s;
        last = s;
    }

    canvas_clear_undone();
    return 0;
}

int canvas_new_stroke_atom(uint16_t x, uint16_t y) {
    if (last->num_atoms == 0) {
        last->num_atoms++;
        last->atoms = malloc(sizeof(stroke_atom));
    } else {
        last->num_atoms++;
        last->atoms = realloc(last->atoms, last->num_atoms * sizeof(stroke_atom));
    }

    if (last->atoms == NULL)
        return 1;

    last->atoms[last->num_atoms - 1].x = x;
    last->atoms[last->num_atoms - 1].y = y;
    
    if (canvas_draw_last_atom() != OK) {
        return 1;
    }

    return 0;
}

int canvas_undo_stroke() {
    stroke *u = last;

    if (u == NULL)
        return 1;
    
    if (u->prev == NULL) {
        first = last = NULL;
    } else {
        u->prev->next = NULL;
        last = u->prev;
    }

    u->prev = undone;
    if (undone != NULL) {
        undone->next = u;
    }
    undone = u;

    if (canvas_redraw_strokes() != OK) 
        return 1;

    return 0;
}

int canvas_redo_stroke() {
    stroke *u = undone;
    if (u == NULL)
        return 1;

    if (u->prev == NULL) {
        undone = NULL;
    } else {
        u->prev->next = NULL;
        undone = u->prev;
    }

    u->prev = last;
    if (last != NULL) {
        last->next = u;
        last = u;
    } else {
        first = last = u;
    }
    

    if (canvas_draw_stroke(u) != OK)
        return 1;

    return 0;
}

int canvas_draw_frame(uint16_t y) {
    uint8_t *buf_pos = (uint8_t*) vg_get_back_buffer().buf + y * vg_get_hres() * vg_get_bytes_per_pixel();
    size_t size = sizeof(uint8_t) * canvas_buf.h_res * canvas_buf.v_res * canvas_buf.bytes_per_pixel;
    memcpy(buf_pos, canvas_buf.buf, size);
    return 0;
}
