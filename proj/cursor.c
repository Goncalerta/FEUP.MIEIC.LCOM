#include <lcom/lcf.h>
#include "cursor.h"
#include "video_gr.h"

static int16_t cursor_x, cursor_y;
//static xpm_image_t cursor_arrow, cursor_write;

void cursor_init() {
    cursor_x = vg_get_hres() / 2;
    cursor_y = vg_get_vres() / 2;
}

int16_t cursor_get_x() {
    return cursor_x;
}

int16_t cursor_get_y() {
    return cursor_y;
}

void update_cursor(struct packet *p) {
    cursor_x += p->delta_x;
    cursor_y -= p->delta_y;

    if (cursor_x < 0)
        cursor_x = 0;
    if (cursor_y < 0)
        cursor_y = 0;
    if (cursor_x > vg_get_hres())
        cursor_x = vg_get_hres();
    if (cursor_y > vg_get_vres())
        cursor_y = vg_get_vres();
}

void cursor_draw(cursor_state state) {
    switch (state) {
    case CURSOR_ARROW:
        // TODO
        // draw_img(cursor_arrow, cursor_x, cursor_y);
        break;
    case CURSOR_PAINT:
        // TODO
        // draw_circle(x, y, CURSOR_RADIUS, selected_color)
        break;
    case CURSOR_WRITE:
        // TODO
        // draw_img(cursor_write, cursor_x, cursor_y);
        break;
    }
}
