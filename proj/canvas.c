#include <lcom/lcf.h>
#include <math.h>
#include "canvas.h"
#include "video_gr.h"
#include "graphics.h"
#include "dispatcher.h"
#include "cursor.h"
#include "game.h"

static canvas_state_t state = CANVAS_STATE_NORMAL;
static stroke *first = NULL, *last = NULL, *undone = NULL;
static frame_buffer_t canvas_buf; // current picture drawn in buffer - copied into vcard back buffer
static bool enabled = false;
static bool initialized = false;

static int canvas_draw_atom_line(stroke_atom atom1, stroke_atom atom2, uint32_t color, uint16_t thickness) {
    uint16_t x1 = atom1.x;
    uint16_t y1 = atom1.y;
    uint16_t x2 = atom2.x;
    uint16_t y2 = atom2.y;

    if (vb_draw_line(canvas_buf, x1, y1, x2, y2, color, thickness) != OK)
        return 1;
    
    return 0;
}

static int canvas_draw_last_atom() {
    if (last->num_atoms == 1) {
        stroke_atom atom = last->atoms[0];
        return vb_draw_circle(canvas_buf, atom.x, atom.y, last->thickness, last->color);
    } else {
        stroke_atom atom1 = last->atoms[last->num_atoms-2];
        stroke_atom atom2 = last->atoms[last->num_atoms-1];
        return canvas_draw_atom_line(atom1, atom2, last->color, last->thickness);
    }
}

static int canvas_draw_stroke(stroke *stroke) {
    stroke_atom first_atom = stroke->atoms[0];
    if (vb_draw_circle(canvas_buf, first_atom.x, first_atom.y, stroke->thickness, stroke->color))
        return 1;

    for (size_t i = 1; i < stroke->num_atoms; i++) {
        if (canvas_draw_atom_line(stroke->atoms[i-1], stroke->atoms[i], stroke->color, stroke->thickness) != OK)
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

int canvas_init(uint16_t width, uint16_t height, bool en) {
    initialized = true;
    enabled = en;
    state = CANVAS_STATE_NORMAL;

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

static void canvas_clear_undone() {
    stroke *current = undone;
    while (current != NULL) {
        stroke *prev = current->prev;
        free(current);
        current = prev;
    }
    
    undone = NULL;
}

void clear_canvas() {
    stroke *current = last;
    while (current != NULL) {
        stroke *prev = current->prev;
        free(current);
        current = prev;
    }
    
    first = NULL;
    last = NULL;

    canvas_clear_undone();
    vb_fill_screen(canvas_buf, 0x00ffffff);
}

void canvas_exit() {
    if (!initialized)
        return;
    initialized = false;
    clear_canvas();
    free(canvas_buf.buf);
}

bool canvas_is_initialized() {
    return initialized;
}

bool canvas_is_enabled() {
    return enabled;
}

int canvas_new_stroke(uint32_t color, uint16_t thickness) {
    stroke *s = malloc(sizeof(stroke));
    s->atoms = NULL;
    s->color = color;
    s->thickness = thickness;
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
        
        if (last->atoms == NULL)
            return 1;
    } else {
        last->num_atoms++;
        stroke_atom *atoms = realloc(last->atoms, last->num_atoms * sizeof(stroke_atom));
        if (atoms == NULL)
            return 1;    
        last->atoms = atoms;
    }

    // If the atom is out of the canvas, it doens't need to be too far
    // from the edge, because it is not visible. For efficiency, the
    // atom is drawn close to the edge, with a tolerance equal to the thickness
    // (so the line goes completely out of the screen).
    if (x > canvas_buf.h_res + last->thickness) 
        x = canvas_buf.h_res + last->thickness;

    if (y > canvas_buf.v_res + last->thickness)
        y = canvas_buf.v_res + last->thickness;
    
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
        return 0;
    
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
    
    if (state == CANVAS_STATE_PRESSING_LB || state == CANVAS_STATE_PRESSING_RB) {
        if (event_new_stroke(true) != OK)
            return 1;
        if (event_new_atom(cursor_get_x(), cursor_get_y()) != OK)
            return 1;
    }

    return 0;
}

int canvas_redo_stroke() {
    stroke *u = undone;
    if (u == NULL)
        return 0;

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

bool canvas_is_hovering(uint16_t x, uint16_t y) {
    return y <= canvas_buf.v_res;
}

canvas_state_t canvas_get_state() {
    return state;
}

int canvas_update_state(bool hovering, bool lb, bool rb) {
    switch (state) {
    case CANVAS_STATE_NORMAL:
        if (hovering && !(lb || rb)) {
            state = CANVAS_STATE_HOVERING;
        }
        break;
    
    case CANVAS_STATE_HOVERING:
        if (hovering) {
            if (lb && rb) {
                state = CANVAS_STATE_NORMAL;
            } else if (lb && !rb) {
                state = CANVAS_STATE_PRESSING_LB;
                if (enabled) {
                    if (event_new_stroke(true) != OK)
                        return 1;
                    if (event_new_atom(cursor_get_x(), cursor_get_y()) != OK)
                        return 1;
                }
            } else if (rb && !lb) {
                state = CANVAS_STATE_PRESSING_RB;
                if (enabled) {
                    if (drawer_toggle_pencil_eraser() != OK)
                        return 1;
                    if (event_new_stroke(false) != OK)
                        return 1;
                    if (event_new_atom(cursor_get_x(), cursor_get_y()) != OK)
                        return 1;
                }
            }
        } else {
            state = CANVAS_STATE_NORMAL;
        }
        break;

    case CANVAS_STATE_PRESSING_LB:
        if (lb && rb) {
            state = CANVAS_STATE_NORMAL;
        } else if (lb && !rb) {
            if (enabled) {
                if (event_new_atom(cursor_get_x(), cursor_get_y()) != OK)
                    return 1;
            }
        } else if (!lb && rb) {
            state = CANVAS_STATE_PRESSING_RB;
            if (enabled) {
                if (drawer_toggle_pencil_eraser() != OK)
                    return 1;
                if (event_new_stroke(false) != OK)
                    return 1;
                if (event_new_atom(cursor_get_x(), cursor_get_y()) != OK)
                    return 1;
            }
        } else {
            state = hovering? CANVAS_STATE_HOVERING : CANVAS_STATE_NORMAL;
        }
        break;

    case CANVAS_STATE_PRESSING_RB:
        if (lb && rb) {
            state = CANVAS_STATE_NORMAL;
            if (enabled) {
                if (drawer_toggle_pencil_eraser() != OK)
                    return 1;
            }
        } else if (rb && !lb) {
            if (enabled) {
                if (event_new_atom(cursor_get_x(), cursor_get_y()) != OK)
                    return 1;
            }
        } else if (!rb && lb) {
            state = CANVAS_STATE_PRESSING_LB;
            if (enabled) {
                if (drawer_toggle_pencil_eraser() != OK)
                    return 1;
                if (event_new_stroke(true) != OK)
                    return 1;
                if (event_new_atom(cursor_get_x(), cursor_get_y()) != OK)
                    return 1;
            }
        } else {
            state = hovering? CANVAS_STATE_HOVERING : CANVAS_STATE_NORMAL;
            if (enabled) {
                if (drawer_toggle_pencil_eraser() != OK)
                    return 1;
            }
        }
        break;
    }

    return 0;
}

int canvas_react_kbd(kbd_event_t kbd_event) {
    if (enabled) {
        if (kbd_event.key == CHAR && kbd_event.char_key == 'Z' && kbd_event.is_ctrl_pressed) {
            if (event_undo() != OK)
                return 1;
        }

        if (kbd_event.key == CHAR && kbd_event.char_key == 'Y' && kbd_event.is_ctrl_pressed) {
            if (event_redo() != OK)
                return 1;
        }

        if (kbd_event.key == CHAR && kbd_event.char_key == 'P' && !kbd_event.is_ctrl_pressed) {
            if (drawer_set_pencil_primary() != OK)
                return 1;
        }

        if (kbd_event.key == CHAR && kbd_event.char_key == 'E' && !kbd_event.is_ctrl_pressed) {
            if (drawer_set_eraser_primary() != OK)
                return 1;
        }

        if (kbd_event.key == CHAR && kbd_event.char_key == 'C' && !kbd_event.is_ctrl_pressed) {
            if (drawer_change_selected_color() != OK)
                return 1;
        }

        if (kbd_event.key == CHAR && kbd_event.char_key == 'T' && !kbd_event.is_ctrl_pressed) {
            if (drawer_change_selected_thickness() != OK)
                return 1;
        }
    }

    return 0;
}
