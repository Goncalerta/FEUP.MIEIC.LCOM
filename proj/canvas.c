#include <lcom/lcf.h>
#include <math.h>
#include "canvas.h"
#include "video_gr.h"
#include "graphics.h"
#include "dispatcher.h"
#include "cursor.h"
#include "game.h"

/** @defgroup canvas canvas
 * @{
 *
 */

#define MAXIMUM_UNDOABLE_ATOMS 50 //1024

static canvas_state_t state = CANVAS_STATE_NORMAL; /**< @brief Canvas current state */
static stroke_t *first = NULL; /**< @brief Reference to the first stroke drawn */
static stroke_t *last = NULL; /**< @brief Reference to the last stroke drawn */
static stroke_t *undone = NULL; /**< @brief Reference to the last stroke undone */
static frame_buffer_t canvas_buf; /**< @brief Buffer with the current content of the canvas drawn to be copied into the back buffer every frame */
static frame_buffer_t canvas_base_buf;
static bool enabled = false; /**< @brief Whether the canvas should allow the user to draw with the cursor */
static bool initialized = false; /**< @brief Whether canvas_init() has been called since last call to canvas_exit() and canvas functions may be safely used */

static size_t undo_atoms_limit_count = 0;

/**
 * @brief Draws a line between two stroke_atom_t to the given buffer.
 * 
 * @param buf buffer to draw
 * @param atom1 the first atom
 * @param atom2 the second atom
 * @param color the color of the line
 * @param thickness the thickness of the line
 * @return Return 0 upon success and non-zero otherwise
 */
static int canvas_draw_atom_line(frame_buffer_t buf, stroke_atom_t atom1, stroke_atom_t atom2, uint32_t color, uint16_t thickness) {
    uint16_t x1 = atom1.x;
    uint16_t y1 = atom1.y;
    uint16_t x2 = atom2.x;
    uint16_t y2 = atom2.y;

    if (vb_draw_line(buf, x1, y1, x2, y2, color, thickness) != OK)
        return 1;
    
    return 0;
}

/**
 * If it is the only stroke_atom_t in the stroke, draw a circle, else connect it with a line to the previous stroke_atom_t.
 * 
 * @brief Draws the last stroke_atom_t in a stroke.
 * 
 * @param buf buffer to draw
 * @return Return 0 upon success and non-zero otherwise
 */
static int canvas_draw_last_atom(frame_buffer_t buf) {
    if (last->num_atoms == 1) {
        stroke_atom_t atom = last->atoms[0];
        return vb_draw_circle(buf, atom.x, atom.y, last->thickness, last->color);
    } else {
        stroke_atom_t atom1 = last->atoms[last->num_atoms-2];
        stroke_atom_t atom2 = last->atoms[last->num_atoms-1];
        return canvas_draw_atom_line(buf, atom1, atom2, last->color, last->thickness);
    }
}

/**
 * @brief Draws complete stroke_t into the canvas buffer.
 * 
 * @param buf buffer to draw
 * @param stroke the stroke to be drawn
 * @return Return 0 upon success and non-zero otherwise
 */
static int canvas_draw_stroke(frame_buffer_t buf, stroke_t *stroke) {
    stroke_atom_t first_atom = stroke->atoms[0];
    if (vb_draw_circle(buf, first_atom.x, first_atom.y, stroke->thickness, stroke->color))
        return 1;

    for (size_t i = 1; i < stroke->num_atoms; i++) {
        if (canvas_draw_atom_line(buf, stroke->atoms[i-1], stroke->atoms[i], stroke->color, stroke->thickness) != OK)
            return 1;
    }

    return 0;
}

static int canvas_draw_excess_strokes(frame_buffer_t buf) {
    while (undo_atoms_limit_count > MAXIMUM_UNDOABLE_ATOMS) {
        stroke_t *s = first;
        first = s->next;
        first->prev = NULL;
        undo_atoms_limit_count -= s->num_atoms;
        if (canvas_draw_stroke(buf, s) != OK) {
            free(s);
            return 1;
        }
        free(s);
    }

    printf("%d\n", undo_atoms_limit_count);
    
    return 0;
}

/**
 * Useful for undoing a stroke.
 * 
 * @brief Redraws all strokes in the canvas buffer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static int canvas_redraw_strokes() {
    memcpy(canvas_buf.buf, canvas_base_buf.buf, 
           sizeof(uint8_t) * canvas_buf.h_res * canvas_buf.v_res * canvas_buf.bytes_per_pixel);

    stroke_t *current = first;
    while (current != NULL) {
        canvas_draw_stroke(canvas_buf, current);

        current = current->next;
    }

    return 0;
}

int canvas_init(uint16_t width, uint16_t height, bool en) {
    canvas_base_buf.h_res = width;
    canvas_base_buf.v_res = height;
    canvas_base_buf.bytes_per_pixel = vg_get_bytes_per_pixel();
    canvas_base_buf.buf = malloc(sizeof(uint8_t) * width * height * canvas_base_buf.bytes_per_pixel);
    if (canvas_base_buf.buf == NULL)
        return 1;
    
    canvas_buf.h_res = width;
    canvas_buf.v_res = height;
    canvas_buf.bytes_per_pixel = vg_get_bytes_per_pixel();
    canvas_buf.buf = malloc(sizeof(uint8_t) * width * height * canvas_buf.bytes_per_pixel);
    if (canvas_buf.buf == NULL) {
        free(canvas_base_buf.buf);
        return 1;
    }
    
    vb_fill_screen(canvas_base_buf, 0x00ffffff);
    memcpy(canvas_buf.buf, canvas_base_buf.buf, sizeof(uint8_t) * width * height * canvas_buf.bytes_per_pixel);

    initialized = true;
    enabled = en;
    state = CANVAS_STATE_NORMAL;

    first = NULL;
    last = NULL;
    undone = NULL;
    undo_atoms_limit_count = 0;

    return 0;
}

/**
 * 
 * @brief Deletes all undone strokes.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
static void canvas_clear_undone() {
    stroke_t *current = undone;
    
    while (current != NULL) {
        stroke_t *prev = current->prev;
        if (prev != NULL) {
            undo_atoms_limit_count -= current->num_atoms;
        }
        
        free(current);
        current = prev;
    }
    printf("CLEAR UNDONE %d\n", undo_atoms_limit_count);
    
    undone = NULL;
}

void clear_canvas() {
    stroke_t *current = last;
    while (current != NULL) {
        stroke_t *prev = current->prev;
        free(current);
        current = prev;
    }
    
    first = NULL;
    last = NULL;

    canvas_clear_undone();
    undo_atoms_limit_count = 0;
    vb_fill_screen(canvas_base_buf, 0x00ffffff);
    memcpy(canvas_buf.buf, canvas_base_buf.buf, 
           sizeof(uint8_t) * canvas_buf.h_res * canvas_buf.v_res * canvas_buf.bytes_per_pixel);
}

void canvas_exit() {
    if (!initialized)
        return;
    initialized = false;
    clear_canvas();
    free(canvas_buf.buf);
    free(canvas_base_buf.buf);
}

bool canvas_is_initialized() {
    return initialized;
}

bool canvas_is_enabled() {
    return enabled;
}

int canvas_new_stroke(uint32_t color, uint16_t thickness) {
    stroke_t *s = malloc(sizeof(stroke_t));
    if (s == NULL)
        return 1;

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
        if (undone == NULL) {
            undo_atoms_limit_count += last->num_atoms;
        }
        
        s->prev = last;
        last->next = s;
        last = s;

        printf("%d\n", undo_atoms_limit_count);
        if (undo_atoms_limit_count > MAXIMUM_UNDOABLE_ATOMS) {
            if (canvas_draw_excess_strokes(canvas_base_buf) != OK)
                return 1;
        }
    }

    canvas_clear_undone();
    return 0;
}

int canvas_new_stroke_atom(uint16_t x, uint16_t y) {
    if (last->num_atoms == 0) {
        last->num_atoms++;
        last->atoms = malloc(sizeof(stroke_atom_t));
        
        if (last->atoms == NULL)
            return 1;
    } else {
        last->num_atoms++;
        stroke_atom_t *atoms = realloc(last->atoms, last->num_atoms * sizeof(stroke_atom_t));
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
    
    if (canvas_draw_last_atom(canvas_buf) != OK)
        return 1;

    return 0;
}

int canvas_undo_stroke() {
    stroke_t *u = last;

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
        if (handle_new_stroke(true) != OK)
            return 1;
        if (handle_new_atom(cursor_get_x(), cursor_get_y()) != OK)
            return 1;
    }

    return 0;
}

int canvas_redo_stroke() {
    stroke_t *u = undone;
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
    

    if (canvas_draw_stroke(canvas_buf, u) != OK)
        return 1;

    return 0;
}

int canvas_draw_frame(uint16_t y) {
    // Canvas width is assumed to occupy the whole screen.
    // This allows the use of memcpy of the whole buffer into the video buffer
    // which helps solving the slow mouse issues in some computers.
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

void canvas_unselect() {
    state = CANVAS_STATE_NORMAL;
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
                    if (handle_new_stroke(true) != OK)
                        return 1;
                    if (handle_new_atom(cursor_get_x(), cursor_get_y()) != OK)
                        return 1;
                }
            } else if (rb && !lb) {
                state = CANVAS_STATE_PRESSING_RB;
                if (enabled) {
                    if (drawer_toggle_pencil_eraser() != OK)
                        return 1;
                    if (handle_new_stroke(false) != OK)
                        return 1;
                    if (handle_new_atom(cursor_get_x(), cursor_get_y()) != OK)
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
                if (handle_new_atom(cursor_get_x(), cursor_get_y()) != OK)
                    return 1;
            }
        } else if (!lb && rb) {
            state = CANVAS_STATE_PRESSING_RB;
            if (enabled) {
                if (drawer_toggle_pencil_eraser() != OK)
                    return 1;
                if (handle_new_stroke(false) != OK)
                    return 1;
                if (handle_new_atom(cursor_get_x(), cursor_get_y()) != OK)
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
                if (handle_new_atom(cursor_get_x(), cursor_get_y()) != OK)
                    return 1;
            }
        } else if (!rb && lb) {
            state = CANVAS_STATE_PRESSING_LB;
            if (enabled) {
                if (drawer_toggle_pencil_eraser() != OK)
                    return 1;
                if (handle_new_stroke(true) != OK)
                    return 1;
                if (handle_new_atom(cursor_get_x(), cursor_get_y()) != OK)
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
            if (handle_undo() != OK)
                return 1;
        }

        if (kbd_event.key == CHAR && kbd_event.char_key == 'Y' && kbd_event.is_ctrl_pressed) {
            if (handle_redo() != OK)
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

/**@}*/
