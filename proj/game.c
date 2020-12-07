#include <lcom/lcf.h>

#include "game.h"
#include "graphics.h"
#include "video_gr.h"
#include "font.h"
#include "dispatcher.h"

#include "xpm/clock_red_left.xpm"
#include "xpm/clock_red_center.xpm"
#include "xpm/clock_red_right.xpm"

#define ROUND_SECONDS 60
#define RED_CLOCK_THRESHOLD 10

static xpm_animation_t clock_frames;
static int clock_frames_timer;
static int round_timer;
static size_t current_clock_frame;

int game_load_assets(enum xpm_image_type type) {
    xpm_load_animation(&clock_frames, type, 3, xpm_clock_red_left, xpm_clock_red_center, xpm_clock_red_right);

    return 0;
}

int game_start_round() {
    current_clock_frame = 1;
    round_timer = ROUND_SECONDS * 60;
    return 0;
}

void game_round_timer_tick() {
    if (round_timer == 0)
        event_end_round();
    else
        round_timer--;

    clock_frames_timer+=2;
    if (clock_frames_timer == 10) {
        clock_frames.current_frame = 0;
    } else if (clock_frames_timer == 30) {
        clock_frames.current_frame = 1;
    } else if (clock_frames_timer == 40) {
        clock_frames.current_frame = 2;
    } else if (clock_frames_timer == 60) {
        clock_frames.current_frame = 1;
        clock_frames_timer = 0;
    }
}

int draw_game_bar() {
    frame_buffer_t buf = vg_get_back_buffer();
    
    if (vb_draw_rectangle(buf, 0, buf.v_res - GAME_BAR_HEIGHT, buf.h_res, GAME_BAR_PADDING, GAME_BAR_COLOR_DARK) != OK)
        return 1;

    if (vb_draw_rectangle(buf, 0, buf.v_res - GAME_BAR_INNER_HEIGHT, buf.h_res, GAME_BAR_INNER_HEIGHT, GAME_BAR_COLOR) != OK)
        return 1;

    if (vb_draw_animation_frame(buf, clock_frames, buf.h_res - 200, 
                                buf.v_res - (GAME_BAR_INNER_HEIGHT + clock_frames.height)/2) != OK)
        return 1;

    char seconds_to_end_round[2];
    sprintf(seconds_to_end_round, "%02d", (round_timer + 59)/60);
    if (font_draw_string(buf, seconds_to_end_round, buf.h_res - 75, 
                         buf.v_res - (GAME_BAR_INNER_HEIGHT + FONT_CHAR_HEIGHT)/2, 0, 2) != OK)
        return 1;

    return 0;
}
