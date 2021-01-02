#include <lcom/lcf.h>
#include "cursor.h"
#include "video_gr.h"
#include "canvas.h"
#include "game.h"
#include "mouse.h"

#include "xpm/cursor_arrow.xpm"
#include "xpm/cursor_write.xpm"
#include "xpm/cursor_disabled.xpm"

static cursor_state_t state = CURSOR_ARROW;
static int16_t cursor_x = 0, cursor_y = 0;
static bool lb = false, rb = false;
static xpm_image_t cursor_arrow, cursor_write, cursor_disabled;

int cursor_init(enum xpm_image_type type) {
    if (xpm_load(xpm_cursor_arrow, type, &cursor_arrow) == NULL)
        return 1;
    if (xpm_load(xpm_cursor_write, type, &cursor_write) == NULL)
        return 1;
    if (xpm_load(xpm_cursor_disabled, type, &cursor_disabled) == NULL)
        return 1;
    cursor_x = vg_get_hres() / 2;
    cursor_y = vg_get_vres() / 2;
    lb = false;
    rb = false;
    
    return 0;
}

void cursor_exit() {
    free(cursor_arrow.bytes);
    free(cursor_write.bytes);
    free(cursor_disabled.bytes);
}

int16_t cursor_get_x() {
    return cursor_x;
}

int16_t cursor_get_y() {
    return cursor_y;
}

bool cursor_get_lb() {
    return lb;
}

bool cursor_get_rb() {
    return rb;
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

void cursor_update_buttons(bool new_lb, bool new_rb) {
    lb = new_lb;
    rb = new_rb;
}

int cursor_draw() {
    switch (state) {
    case CURSOR_ARROW:
        return vb_draw_img(vg_get_back_buffer(), cursor_arrow, cursor_x - 12, cursor_y - 12);
    case CURSOR_PAINT:
        if (vb_draw_circle(vg_get_back_buffer(), cursor_x, cursor_y, drawer_get_selected_thickness()+2, 0x00393939) != OK) 
            return 1; 
        if (vb_draw_circle(vg_get_back_buffer(), cursor_x, cursor_y, drawer_get_selected_thickness(), drawer_get_selected_color()) != OK) 
            return 1; 
        return 0;
    case CURSOR_WRITE:
        return vb_draw_img(vg_get_back_buffer(), cursor_write, cursor_x - 12, cursor_y - 12);
    case CURSOR_DISABLED:
        return vb_draw_img(vg_get_back_buffer(), cursor_disabled, cursor_x - 12, cursor_y - 12);
    }
}

void cursor_set_state(cursor_state_t new_state) {
    state = new_state;
}
