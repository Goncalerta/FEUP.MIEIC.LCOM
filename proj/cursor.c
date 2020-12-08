#include <lcom/lcf.h>
#include "cursor.h"
#include "video_gr.h"
#include "canvas.h"
#include "game.h"

#include "xpm/cursor_arrow.xpm"
#include "xpm/cursor_write.xpm"

static cursor_state state;
static int16_t cursor_x, cursor_y;
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

int cursor_draw() {
    switch (state) {
    case CURSOR_ARROW:
        return vb_draw_img(vg_get_back_buffer(), cursor_arrow, 0, 0, 24, 24, cursor_x - 12, cursor_y - 12);
    case CURSOR_PAINT:
        if (vb_draw_circle(vg_get_back_buffer(), cursor_x, cursor_y, game_get_selected_thickness()+2, 0x00393939) != OK) 
            return 1; 
        if (vb_draw_circle(vg_get_back_buffer(), cursor_x, cursor_y, game_get_selected_thickness(), game_get_selected_color()) != OK) 
            return 1; 
        return 0;
    case CURSOR_WRITE:
        return vb_draw_img(vg_get_back_buffer(), cursor_write, 0, 0, 24, 24, cursor_x - 12, cursor_y - 12);
    }
}

void cursor_set_state(cursor_state new_state) {
    state = new_state;
}
