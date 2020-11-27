#include <lcom/lcf.h>
#include <math.h>
#include "canvas.h"
#include "video_gr.h"
#include "graphics.h"

//#define CANVAS_WIDTH 100
//#define CANVAS_HEIGHT 100
//#define CANVAS_STROKE 5
//#define TOLERANCE 10

static stroke *first, *last, *undone;
static video_buffer_t canvas_buf; // current picture drawn in buffer - copied into vcard back buffer
static video_buffer_t base_buf; // base buffer - can't be undone any further

int canvas_init(uint16_t width, uint16_t height) {
    canvas_buf.h_res = base_buf.h_res = width;
    canvas_buf.v_res = base_buf.v_res = height;

    canvas_buf.bits_per_pixel = base_buf.bits_per_pixel = vg_get_bits_per_pixel();
    size_t buffer_size = sizeof(uint8_t) * width * height * ceil(base_buf.bits_per_pixel/8.0);
    canvas_buf.buf = malloc(buffer_size);
    base_buf.buf = malloc(buffer_size);
    vb_fill_screen(canvas_buf, 0x00ffffff);
    vb_fill_screen(base_buf, 0x00ffffff);

    return 0;
}

int canvas_exit() {
    if (clear_canvas() != OK)
        return 1;
    free(canvas_buf.buf);
    free(base_buf.buf);
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

int canvas_new_stroke_atom(uint16_t x, uint16_t y) {
    if (last->num_atoms == 0) {
        last->num_atoms++;
        last->atoms = malloc(sizeof(stroke_atom));
    } else {
        //uint16_t last_x = last->atoms[last->num_atoms - 1].y;
        //uint16_t last_y = last->atoms[last->num_atoms - 1].y;
        //if (x - last_x < TOLERANCE && y - last_y < TOLERANCE) 
        //    return 0;

        last->num_atoms++;
        last->atoms = realloc(last->atoms, last->num_atoms * sizeof(stroke_atom));
    }

    if (last->atoms == NULL)
        return 1;

    last->atoms[last->num_atoms - 1].x = x;
    last->atoms[last->num_atoms - 1].y = y;
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
    }
    last = u;

    return 0;
}

int canvas_draw_atom_line(stroke_atom atom1, stroke_atom atom2, uint32_t color) {
    uint16_t x1 = atom1.x;
    uint16_t y1 = atom1.y;
    uint16_t x2 = atom2.x;
    uint16_t y2 = atom2.y;

    if (vb_draw_line(canvas_buf, x1, y1, x2, y2, color) != OK)
        return 1;
    
    return 0;
}

int canvas_draw_last_atom() {
    if (last->num_atoms == 1) return 0;
    return canvas_draw_atom_line(last->atoms[last->num_atoms-2], last->atoms[last->num_atoms-1], last->color);
}

int canvas_draw_strokes() {
    memcpy(canvas_buf.buf, base_buf.buf, sizeof(uint8_t) * canvas_buf.h_res * canvas_buf.v_res * canvas_buf.bits_per_pixel / 8);

    stroke *current = first;
    while (current != NULL) {
        for (size_t i = 1; i < current->num_atoms; i++) {
            if (canvas_draw_atom_line(current->atoms[i-1], current->atoms[i], current->color) != OK)
                return 1;
        }

        current = current->next;
    }

    return 0;
}

int canvas_draw_frame(uint16_t y) {
    // uint8_t bytes_per_pixel = ceil(canvas_buf.bits_per_pixel/8.0);

    // uint8_t *pixel_mem_pos = (uint8_t*) vg_get_back_buffer().buf;
    // uint8_t *pixel_mem_pos2 = (uint8_t*) canvas_buf.buf;


    // for (int32_t px = 0; px < canvas_buf.h_res * canvas_buf.v_res * bytes_per_pixel; px += bytes_per_pixel) {
    //     for (uint8_t i = 0; i < bytes_per_pixel; i++) {
    //         pixel_mem_pos[px + i] = pixel_mem_pos2[px + i];
    //     }
    // }
    memcpy(vg_get_back_buffer().buf /*+ y * vg_get_hres()*/, canvas_buf.buf, sizeof(uint8_t) * canvas_buf.h_res * canvas_buf.v_res * canvas_buf.bits_per_pixel / 8);
    return 0;
}
