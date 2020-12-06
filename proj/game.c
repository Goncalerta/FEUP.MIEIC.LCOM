#include <lcom/lcf.h>

#include "game.h"
#include "graphics.h"
#include "video_gr.h"

int draw_game_bar() {
    frame_buffer_t buf = vg_get_back_buffer();
    if (vb_draw_rectangle(buf, 0, buf.v_res - GAME_BAR_HEIGHT, buf.h_res, GAME_BAR_HEIGHT, GAME_BAR_COLOR) != OK)
        return 1;

    return 0;
}
