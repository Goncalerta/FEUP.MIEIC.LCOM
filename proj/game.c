#include <lcom/lcf.h>

#include "game.h"
#include "graphics.h"
#include "video_gr.h"
#include "font.h"
#include "dispatcher.h"
#include "textbox.h"
#include "button.h"

#include "xpm/clock_left.xpm"
#include "xpm/clock_center.xpm"
#include "xpm/clock_right.xpm"
#include "xpm/pencil.xpm"
#include "xpm/eraser.xpm"
#include "xpm/undo_arrow.xpm"
#include "xpm/redo_arrow.xpm"

#define ROUND_SECONDS 60
#define BUTTONS_LEN 75

static xpm_animation_t clock_frames;
static const uint32_t canvas_pallete[] = {
    // black, blue, red, green, yellow, pink, purple, orange, brown, gray
    0x000000, 0x1E88E5, 0xD50000, 0x2E7D32, 0xFFEB3B, 0xEC407A, 0x4A148C, 0xFF6D00, 0x5d4037, 0x424242
};
static const uint16_t valid_thickness[] = {
    1, 10, 20
};
static bool is_pencil_primary;
static size_t selected_color;
static size_t selected_thickness;
static int clock_frames_timer;
static int round_timer;
static size_t current_clock_frame;
static int score;
static int round;

static button_t b_pencil, b_eraser, b_color, b_thickness, b_undo, b_redo;
int game_change_selected_color() {
    selected_color++;
    if (selected_color >= 10) {
        selected_color = 0;
    }
    button_set_circle_icon(&b_color, 15, canvas_pallete[selected_color]);
    return 0;
}

int game_change_selected_thickness() {
    selected_thickness++;
    if (selected_thickness >= 3) {
        selected_thickness = 0;
    }
    button_set_circle_icon(&b_thickness, valid_thickness[selected_thickness], 0x000000);
    return 0;
}

uint32_t game_get_selected_color() {
    if (is_pencil_primary)
        return canvas_pallete[selected_color];
    else
        return 0x00FFFFFF;
}

uint16_t game_get_selected_thickness() {
    return valid_thickness[selected_thickness];
}

bool game_is_pencil_primary() {
    return is_pencil_primary;
}

void game_toggle_pencil_eraser() {
    if (is_pencil_primary) {
        game_set_eraser_primary();
    } else {
        game_set_pencil_primary();
    }
}

int game_set_pencil_primary() {
    is_pencil_primary = true;
    button_set_border_active(&b_pencil);
    button_unset_border_active(&b_eraser);
    return 0;
}

int game_set_eraser_primary() {
    is_pencil_primary = false;
    button_set_border_active(&b_eraser);
    button_unset_border_active(&b_pencil);
    return 0;
}

int game_load_assets(enum xpm_image_type type) {
    xpm_load_animation(&clock_frames, type, 3, 
                       xpm_clock_red_left, xpm_clock_red_center, xpm_clock_red_right);
    frame_buffer_t buf = vg_get_back_buffer();

    selected_color = 0;
    selected_thickness = 1;
    is_pencil_primary = true;

    uint16_t button_margin = 10;
    uint16_t button_y = button_margin;
    xpm_image_t pencil;
    xpm_load(xpm_pencil, type, &pencil);
    new_button(&b_pencil, buf.h_res - BUTTONS_LEN - button_margin, button_y, BUTTONS_LEN, BUTTONS_LEN, game_set_pencil_primary);
    button_set_xpm_icon(&b_pencil, pencil);
    button_set_border_active(&b_pencil);

    button_y += BUTTONS_LEN + button_margin;
    xpm_image_t eraser;
    xpm_load(xpm_eraser, type, &eraser);
    new_button(&b_eraser, buf.h_res - BUTTONS_LEN - button_margin, button_y, BUTTONS_LEN, BUTTONS_LEN, game_set_eraser_primary);
    button_set_xpm_icon(&b_eraser, eraser);

    button_y += BUTTONS_LEN + button_margin;
    new_button(&b_color, buf.h_res - BUTTONS_LEN - button_margin, button_y, BUTTONS_LEN, BUTTONS_LEN, game_change_selected_color);
    button_set_circle_icon(&b_color, 15, canvas_pallete[selected_color]);

    button_y += BUTTONS_LEN + button_margin;
    new_button(&b_thickness, buf.h_res - BUTTONS_LEN - button_margin, button_y, BUTTONS_LEN, BUTTONS_LEN, game_change_selected_thickness);
    button_set_circle_icon(&b_thickness, valid_thickness[selected_thickness], 0x000000);

    button_y += BUTTONS_LEN + button_margin;
    xpm_image_t undo_arrow;
    xpm_load(xpm_undo_arrow, type, &undo_arrow);
    new_button(&b_undo, buf.h_res - BUTTONS_LEN - button_margin, button_y, BUTTONS_LEN, BUTTONS_LEN, event_undo);
    button_set_xpm_icon(&b_undo, undo_arrow);

    button_y += BUTTONS_LEN + button_margin;
    xpm_image_t redo_arrow;
    xpm_load(xpm_redo_arrow, type, &redo_arrow);
    new_button(&b_redo, buf.h_res - BUTTONS_LEN - button_margin, button_y, BUTTONS_LEN, BUTTONS_LEN, event_redo);
    button_set_xpm_icon(&b_redo, redo_arrow);

    dispatcher_bind_buttons(6, &b_pencil, &b_eraser, &b_color, &b_thickness, &b_undo, &b_redo);

    score = 0;
    round = 0;
    return 0;
}

int game_start_round() {
    text_box_initiate(GUESSER);
    text_box_select(GUESSER);
    round++;
    current_clock_frame = 1;
    clock_frames_timer = 0;
    round_timer = ROUND_SECONDS * 60;
    return 0;
}

void game_round_timer_tick() {
    if (round_timer == 0)
        event_end_round();
    else
       round_timer--;

    clock_frames_timer++;
    if (clock_frames_timer == 10) {
        clock_frames.current_frame = 0;
    } else if (clock_frames_timer == 30) {
        clock_frames.current_frame = 1;
    } else if (clock_frames_timer == 40) {
        clock_frames.current_frame = 2;
    } else if (clock_frames_timer >= 60) {
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


    int score_margin_small = 5;
    int score_margin_big = (GAME_BAR_INNER_HEIGHT - 4*FONT_CHAR_HEIGHT - 2*score_margin_small) / 3;
    int y = buf.v_res - GAME_BAR_INNER_HEIGHT + score_margin_big;
    if (font_draw_string(buf, "SCORE", buf.h_res - 350, 
                         y, 0, 5) != OK)
        return 1;

    char score_display[5];
    y += FONT_CHAR_HEIGHT + score_margin_small;
    sprintf(score_display, "%05d", score);
    if (font_draw_string(buf, score_display, buf.h_res - 350, 
                         y, 0, 5) != OK)
        return 1;

    y += FONT_CHAR_HEIGHT + score_margin_big;
    if (font_draw_string(buf, "ROUND", buf.h_res - 350, 
                         y, 0, 5) != OK)
        return 1;

    char round_display[5];
    y += FONT_CHAR_HEIGHT + score_margin_small;
    sprintf(round_display, "%5d", round);
    if (font_draw_string(buf, round_display, buf.h_res - 350, 
                         y, 0, 5) != OK)
        return 1;

    text_box_draw(buf, GUESSER, true);
    button_draw(buf, b_pencil);
    button_draw(buf, b_eraser);
    button_draw(buf, b_color);
    button_draw(buf, b_thickness);
    button_draw(buf, b_undo);
    button_draw(buf, b_redo);

    return 0;
}