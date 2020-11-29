#include <lcom/lcf.h>
#include "cursor.h"
#include "video_gr.h"
#include "canvas.h"

#include "xpm/cursor_arrow.xpm"
#include "xpm/cursor_write.xpm"

static int16_t cursor_x, cursor_y;
static bool is_lb_pressed = false, is_rb_pressed = false;
static xpm_image_t cursor_arrow, cursor_write;

void cursor_init(enum xpm_image_type type) {
    xpm_load(xpm_cursor_arrow, type, &cursor_arrow);
    xpm_load(xpm_cursor_write, type, &cursor_write);
    cursor_x = vg_get_hres() / 2;
    cursor_y = vg_get_vres() / 2;
}

int16_t cursor_get_x() {
    return cursor_x;
}

int16_t cursor_get_y() {
    return cursor_y;
}

void cursor_move(int16_t dx, int16_t dy) {
    cursor_x += dx;
    cursor_y -= dy;

    if (cursor_x < 0)
        cursor_x = 0;
    if (cursor_y < 0)
        cursor_y = 0;
    if (cursor_x > vg_get_hres())
        cursor_x = vg_get_hres();
    if (cursor_y > vg_get_vres())
        cursor_y = vg_get_vres();
}

bool cursor_is_lb_pressed() {
    return is_lb_pressed;
}

bool cursor_is_rb_pressed() {
    return is_rb_pressed;
}

bool cursor_set_lb_state(bool pressed) {
    if (pressed) {
        if (!is_lb_pressed) {
            is_lb_pressed = true;
            return true;
        }
    } else {
        if (is_lb_pressed) {
            is_lb_pressed = false;
            return true;
        }
    }
    return false;
}

bool cursor_set_rb_state(bool pressed) {
    if (pressed) {
        if (!is_rb_pressed) {
            is_rb_pressed = true;
            return true;
        }
    } else {
        if (is_rb_pressed) {
            is_rb_pressed = false;
            return true;
        }
    }

    return false;
}

int cursor_draw(cursor_state state) {
    switch (state) {
    case CURSOR_ARROW:
        return vb_draw_img(vg_get_back_buffer(), cursor_arrow, 0, 0, 24, 24, cursor_x - 12, cursor_y - 12);
    case CURSOR_PAINT:
        return vb_draw_circle(vg_get_back_buffer(), cursor_x, cursor_y, 10, 0x000033ff); // TODO color and thickness from game state
    case CURSOR_WRITE:
        return vb_draw_img(vg_get_back_buffer(), cursor_write, 0, 0, 24, 24, cursor_x - 12, cursor_y - 12);
    }
}
