#include <lcom/lcf.h>
#include <stdlib.h> 

#include "game.h"
#include "graphics.h"
#include "video_gr.h"
#include "font.h"
#include "dispatcher.h"
#include "textbox.h"
#include "button.h"
#include "clue.h"
#include "menu.h"
#include "rtc.h"

#include "xpm/clock_left.xpm"
#include "xpm/clock_center.xpm"
#include "xpm/clock_right.xpm"
#include "xpm/pencil.xpm"
#include "xpm/eraser.xpm"
#include "xpm/undo_arrow.xpm"
#include "xpm/redo_arrow.xpm"
#include "xpm/tick.xpm"
#include "xpm/cross.xpm"
#include "xpm/correct.xpm"
#include "xpm/gameover.xpm"

#define MAX_SCORE 99999
#define TICKS_PER_SECOND 2
#define ROUND_SECONDS 60
#define ROUND_TICKS ((ROUND_SECONDS) * (TICKS_PER_SECOND))
#define WRONG_GUESS_PENALTY 5
#define BUTTONS_LEN 75
#define GUESS_CHARACTER_LIMIT 14

#define WORD_LIST_SIZE 46
static char *word_list[WORD_LIST_SIZE] = {
    "HOUSE", "TORNADO", "SHOELACE", "TRUCK", "FEAR", "CAREER", "LAKE", "CHRISTMAS",
    "WALLET", "BALL", "IMAGINATION", "HEAL", "MIND", "ROAR", "WEATHER", "EAT", "CAT",
    "WORLD", "WHISPER", "MOVIE", "THEATHER", "DOG", "TRIP", "UNIVERSE", "FOOTBALL",
    "VOLLEYBALL", "PORTUGAL", "CHOCOLATE", "BEAUTIFUL", "DANGER", "ACCIDENT",
    "DEMOCRACY", "FAMILY", "ALIEN", "MARS", "VENUS", "JUPITER", "SUN", "BOOK",
    "PEN", "UNIVERSITY", "FISH", "DOCTOR", "SPIDER", "NEWSPAPER", "HALLOWEEN"
};

#define NUM_COLORS_AVAILABLE 10
static const uint32_t canvas_pallete[NUM_COLORS_AVAILABLE] = {
    // black, blue, red, green, yellow, pink, purple, orange, brown, gray
    0x000000, 0x1E88E5, 0xD50000, 0x2E7D32, 0xFFEB3B, 0xEC407A, 0x4A148C, 0xFF6D00, 0x5d4037, 0x424242
};

#define NUM_THICKNESSES_AVAILABLE 3
static const uint16_t valid_thickness[NUM_THICKNESSES_AVAILABLE] = {
    1, 10, 20
};

typedef enum game_state_t {
    ROUND_UNSTARTED,
    ROUND_ONGOING,
    GAME_OVER,
    TIMES_UP,
    ROUND_CORRECT_GUESS
} game_state_t;

typedef struct drawer_t {
    bool is_pencil_primary;
    size_t selected_color;
    size_t selected_thickness;
    button_t b_pencil, b_eraser, b_color, b_thickness, b_undo, b_redo;
} drawer_t;

typedef struct guesser_t {
    text_box_t text_box;
} guesser_t;

typedef union role_attr_t {
    drawer_t *drawer;
    guesser_t *guesser;
} role_attr_t;

typedef struct guess_t {
    char *guess;
    bool correct;
} guess_t;

typedef struct round_t {
    bool other_player_game_over;
    int round_timer;
    int ticker;

    // GUESSES
    size_t num_guesses;
    guess_t guesses[MAX_GUESSES];
    const char *correct_guess;
    
    // CLUES
    word_clue_t word_clue;

    // ROLE
    role_t role;
    role_attr_t attr;
} round_t;

typedef struct game_t {
    game_state_t state;
    round_t *round;
    uint32_t score;
    uint32_t round_number;
} game_t;

static const rtc_alarm_time_t clue_time_interval = {.hours = 0, .minutes = 0, .seconds = 16};
static const rtc_alarm_time_t end_round_delay = {.hours = 0, .minutes = 0, .seconds = 3};

static xpm_image_t tick_img, cross_img;
static xpm_image_t correct_message, game_over_message;
static xpm_image_t pencil, eraser, undo_arrow, redo_arrow;
static xpm_animation_t clock_frames;

static game_t *game = NULL;

void get_random_word(const char **word) {
    *word = word_list[rand() % WORD_LIST_SIZE];
}

int game_load_assets(enum xpm_image_type type) {
    if (xpm_load_animation(&clock_frames, type, 3, 
                           xpm_clock_red_left, xpm_clock_red_center, xpm_clock_red_right) != OK)
        return 1;

    if (xpm_load(xpm_tick, type, &tick_img) == NULL)
        return 1;
    
    if (xpm_load(xpm_cross, type, &cross_img) == NULL)
        return 1;
    
    if (xpm_load(xpm_correct, type, &correct_message) == NULL)
        return 1;

    if (xpm_load(xpm_gameover, type, &game_over_message) == NULL)
        return 1;
    
    if (xpm_load(xpm_pencil, type, &pencil) == NULL)
        return 1;
    
    if (xpm_load(xpm_eraser, type, &eraser) == NULL)
        return 1;
    
    if (xpm_load(xpm_undo_arrow, type, &undo_arrow) == NULL)
        return 1;
    
    if (xpm_load(xpm_redo_arrow, type, &redo_arrow) == NULL)
        return 1;

    return 0;
}

void game_unload_assets() {
    delete_game();
    free(tick_img.bytes);
    free(cross_img.bytes);
    free(correct_message.bytes);
    free(game_over_message.bytes);
    free(pencil.bytes);
    free(eraser.bytes);
    free(undo_arrow.bytes);
    free(redo_arrow.bytes);
    xpm_unload_animation(&clock_frames);
}

int new_game() {
    game = malloc(sizeof(game_t));
    if (game == NULL)
        return 1;
    game->state = ROUND_UNSTARTED;
    game->round_number = 0;
    game->score = 0;
    game->round = NULL;
    
    return 0;
}

void delete_game() {
    if (game != NULL) {
        game_delete_round();
        free(game);
        game = NULL;
    }
}

static int init_buttons(drawer_t *drawer) {
    frame_buffer_t buf = vg_get_back_buffer();
    uint16_t button_margin = 10;


    uint16_t button_y = button_margin;
    if (new_button(&drawer->b_pencil, buf.h_res - BUTTONS_LEN - button_margin, button_y, 
                   BUTTONS_LEN, BUTTONS_LEN, drawer_set_pencil_primary) != OK)
        return 1;
    
    button_set_xpm_icon(&drawer->b_pencil, pencil);
    button_set_border_active(&drawer->b_pencil);

    
    button_y += BUTTONS_LEN + button_margin;
    if (new_button(&drawer->b_eraser, buf.h_res - BUTTONS_LEN - button_margin, button_y, 
                   BUTTONS_LEN, BUTTONS_LEN, drawer_set_eraser_primary) != OK)
        return 1;
    
    button_set_xpm_icon(&drawer->b_eraser, eraser);

    
    button_y += BUTTONS_LEN + button_margin;
    if (new_button(&drawer->b_color, buf.h_res - BUTTONS_LEN - button_margin, button_y, 
                   BUTTONS_LEN, BUTTONS_LEN, drawer_change_selected_color) != OK)
        return 1;
    
    button_set_circle_icon(&drawer->b_color, BUTTON_CIRCLE_RADIUS_DEFAULT, canvas_pallete[drawer->selected_color]);

    
    button_y += BUTTONS_LEN + button_margin;
    if (new_button(&drawer->b_thickness, buf.h_res - BUTTONS_LEN - button_margin, button_y, 
                   BUTTONS_LEN, BUTTONS_LEN, drawer_change_selected_thickness) != OK)
        return 1;
    
    button_set_circle_icon(&drawer->b_thickness, valid_thickness[drawer->selected_thickness], BUTTON_CIRCLE_DEFAULT_COLOR);

    button_y += BUTTONS_LEN + button_margin;
    if (new_button(&drawer->b_undo, buf.h_res - BUTTONS_LEN - button_margin, 
                   button_y, BUTTONS_LEN, BUTTONS_LEN, event_undo) != OK)
        return 1;
    button_set_xpm_icon(&drawer->b_undo, undo_arrow);

    button_y += BUTTONS_LEN + button_margin;
    if (new_button(&drawer->b_redo, buf.h_res - BUTTONS_LEN - button_margin, button_y, 
                   BUTTONS_LEN, BUTTONS_LEN, event_redo) != OK)
        return 1;
    button_set_xpm_icon(&drawer->b_redo, redo_arrow);

    return 0;
}

role_t game_get_role() {
    return game->round->role;
}

uint32_t game_get_round_number() {
    return game->round_number;
}

uint32_t game_get_score() {
    return game->score;
}

const char *game_get_correct_word() {
    return game->round->correct_guess;
}

bool game_is_round_unstarted() {
    return game != NULL && game->round != NULL && game->state == ROUND_UNSTARTED;
}

bool game_is_round_ongoing() {
    return game != NULL && game->round != NULL && game->state == ROUND_ONGOING;
}

bool game_is_round_ongoing_or_tolerance() {
    return game != NULL && game->round != NULL && (game->state == ROUND_ONGOING || game->state == TIMES_UP);
}

bool game_is_round_won() {
    return game != NULL && game->round != NULL && game->state == ROUND_CORRECT_GUESS;
}

bool game_may_create_new_round() {
    return game != NULL && (game->round == NULL || game->state == GAME_OVER || game->state == ROUND_CORRECT_GUESS);
}

bool game_is_over() {
    return game == NULL || game->state == GAME_OVER;
}

void game_set_over() {
    if (game != NULL)
        game->state = GAME_OVER;
}

static int init_text_box(guesser_t *guesser) {
    if (new_text_box(&game->round->attr.guesser->text_box, TEXT_BOX_GUESSER_X + 4, TEXT_BOX_GUESSER_Y, 
                     TEXT_BOX_GUESSER_DISPLAY_SIZE, event_guess_word) != OK)
        return 1;
    return 0;
}

int game_new_round(role_t role, const char *word) {
    if (!game_may_create_new_round())
        return 1;
    if (game->round != NULL) {
        game_delete_round();
    }

    game->round_number++;
    game->state = ROUND_UNSTARTED;
    game->round = malloc(sizeof(round_t));
    if (game->round == NULL)
        return 1;

    game->round->other_player_game_over = false;
    game->round->ticker = 0;
    game->round->round_timer = ROUND_TICKS;
    game->round->num_guesses = 0;
    game->round->correct_guess = word;
    game->round->role = role;
    switch (role) {
    case DRAWER:
        game->round->attr.drawer = malloc(sizeof(drawer_t));
        if (game->round->attr.drawer == NULL)
            return 1;
        game->round->attr.drawer->is_pencil_primary = true;
        game->round->attr.drawer->selected_color = 0;
        game->round->attr.drawer->selected_thickness = 1;
        if (init_buttons(game->round->attr.drawer) != OK)
            return 1;
        break;

    case GUESSER:
        game->round->attr.guesser = malloc(sizeof(guesser_t));
        if (game->round->attr.guesser == NULL)
            return 1;
        if (init_text_box(game->round->attr.guesser) != OK)
            return 1;
        break;

    default:
        return 1;
    }

    if (new_word_clue(&game->round->word_clue, word) != OK)
        return 1;

    return 0;
}

void game_delete_round() {
    if (game == NULL || game->round == NULL)
        return;

    for (size_t i = 0; i < game->round->num_guesses; i++) {
        free(game->round->guesses[i].guess);
    }
    delete_word_clue(&game->round->word_clue);
    
    switch (game->round->role) {
    case DRAWER:
        free(game->round->attr.drawer);
        break;
    case GUESSER:
        delete_text_box(&game->round->attr.guesser->text_box);
        free(game->round->attr.guesser);
        break;
    default:
        break;
    }
    free(game->round);
    game->round = NULL;
}

int game_resume() {
    if (game == NULL || game->round == NULL)
        return 1;

    if (dispatcher_reset_bindings() != OK)
        return 1;
    switch (game->round->role) {
    case DRAWER:
        if (dispatcher_bind_buttons(6, 
                                    &game->round->attr.drawer->b_pencil, 
                                    &game->round->attr.drawer->b_eraser, 
                                    &game->round->attr.drawer->b_color, 
                                    &game->round->attr.drawer->b_thickness, 
                                    &game->round->attr.drawer->b_undo, 
                                    &game->round->attr.drawer->b_redo) != OK)
            return 1;
        break;
    case GUESSER:
        if (dispatcher_bind_text_boxes(1, &game->round->attr.guesser->text_box) != OK)
            return 1;
        break;
    default:
        break;
    }

    if (dispatcher_bind_canvas(true) != OK)
        return 1;

    menu_set_game_screen();
    if (event_update_cursor_state() != OK)
        return 1;

    return 0;
}

int game_start_round() {
    if (game == NULL || game->round == NULL)
        return 1;

    if (game_resume() != OK)
        return 1;

    if (game->round->role == DRAWER) {
        if (rtc_set_alarm_in(clue_time_interval) != OK)
            return 1;
    }
    
    game->state = ROUND_ONGOING;
    return 0;
}

static int game_draw_bar() {
    if (game == NULL || game->round == NULL)
        return 1;

    frame_buffer_t buf = vg_get_back_buffer();
    
    // Gamebar
    if (vb_draw_rectangle(buf, 0, buf.v_res - GAME_BAR_HEIGHT, buf.h_res, GAME_BAR_PADDING, GAME_BAR_COLOR_DARK) != OK)
        return 1;

    if (vb_draw_rectangle(buf, 0, buf.v_res - GAME_BAR_INNER_HEIGHT, buf.h_res, GAME_BAR_INNER_HEIGHT, GAME_BAR_COLOR) != OK)
        return 1;


    // Clock
    if (vb_draw_animation_frame(buf, clock_frames, buf.h_res - 200, 
                                buf.v_res - (GAME_BAR_INNER_HEIGHT + clock_frames.height)/2) != OK)
        return 1;

    char seconds_to_end_round[3];
    sprintf(seconds_to_end_round, "%02d", (game->round->round_timer + TICKS_PER_SECOND - 1) / TICKS_PER_SECOND);
    
    if (font_draw_string(buf, seconds_to_end_round, buf.h_res - 75, 
                         buf.v_res - (GAME_BAR_INNER_HEIGHT + FONT_CHAR_HEIGHT) / 2) != OK)
        return 1;


    // Score and round number
    int score_round_column_x_from_right = 350;
    int score_margin_small = 5;
    int score_margin_big = (GAME_BAR_INNER_HEIGHT - 4 * FONT_CHAR_HEIGHT - 2 * score_margin_small) / 3;
    int y = buf.v_res - GAME_BAR_INNER_HEIGHT + score_margin_big;
    if (font_draw_string(buf, "SCORE", buf.h_res - score_round_column_x_from_right, y) != OK)
        return 1;

    char score_display[6];
    y += FONT_CHAR_HEIGHT + score_margin_small;
    sprintf(score_display, "%05d", game->score);
    if (font_draw_string(buf, score_display, buf.h_res - score_round_column_x_from_right, y) != OK)
        return 1;

    y += FONT_CHAR_HEIGHT + score_margin_big;
    if (font_draw_string(buf, "ROUND", buf.h_res - score_round_column_x_from_right, y) != OK)
        return 1;

    char round_display[6];
    y += FONT_CHAR_HEIGHT + score_margin_small;
    sprintf(round_display, "%5d", game->round_number);
    if (font_draw_string(buf, round_display, buf.h_res - score_round_column_x_from_right, y) != OK)
        return 1;


    // Text box and word
    switch (game->round->role) {
    case DRAWER:
        if (font_draw_string(buf, "DRAW THE WORD", TEXT_BOX_GUESSER_X, 670) != OK)
            return 1;
        
        if (font_draw_string(buf, game->round->correct_guess, TEXT_BOX_GUESSER_X, TEXT_BOX_GUESSER_Y) != OK)
            return 1;

        break;
    case GUESSER:
        if (font_draw_string(buf, "GUESS THE WORD", TEXT_BOX_GUESSER_X, 670) != OK)
            return 1;

        if (text_box_draw(buf, game->round->attr.guesser->text_box) != OK)
            return 1;

        break;
    default:
        return 1;
    }


    // Guesses
    int guesses_column_x = 350;
    y = buf.v_res - GAME_BAR_INNER_HEIGHT + 7;
    for (size_t i = 0; i < game->round->num_guesses; i++) {
        if (game->round->guesses[i].correct) {
            if (vb_draw_img(buf, tick_img, guesses_column_x, y) != OK)
                return 1;
        } else {
            if (vb_draw_img(buf, cross_img, guesses_column_x, y) != OK)
                return 1;
        }

        char guess[GUESS_CHARACTER_LIMIT + 1] = "";
        if (strlen(game->round->guesses[i].guess) <= GUESS_CHARACTER_LIMIT) {
            strcpy(guess, game->round->guesses[i].guess);
        } else {
            strncpy(guess, game->round->guesses[i].guess, GUESS_CHARACTER_LIMIT - 3);
            guess[GUESS_CHARACTER_LIMIT - 1] = '.';
            guess[GUESS_CHARACTER_LIMIT - 2] = '.';
            guess[GUESS_CHARACTER_LIMIT - 3] = '.';
        }
        if (font_draw_string(buf, guess, guesses_column_x + 21, y) != OK)
            return 1;

        y += FONT_CHAR_HEIGHT + 10;
    }

    return 0;
}

static int game_draw_buttons(drawer_t *drawer) {
    frame_buffer_t buf = vg_get_back_buffer();

    if (button_draw(buf, drawer->b_pencil) != OK)
        return 1;
    if (button_draw(buf, drawer->b_eraser) != OK)
        return 1;
    if (button_draw(buf, drawer->b_color) != OK)
        return 1;
    if (button_draw(buf, drawer->b_thickness) != OK)
        return 1;
    if (button_draw(buf, drawer->b_undo) != OK)
        return 1;
    if (button_draw(buf, drawer->b_redo) != OK)
        return 1;

    return 0;
}

int game_draw() {
    if (game == NULL || game->round == NULL)
        return 1;

    if (game_draw_bar() != OK)
        return 1;

    frame_buffer_t buf = vg_get_back_buffer();

    if (game->state != ROUND_UNSTARTED) {
        if (word_clue_draw(&game->round->word_clue, buf, (buf.h_res - game->round->word_clue.width) / 2, 40) != OK)
            return 1;
    }

    if (game->state == GAME_OVER) {
        // Offset gives an effect of shaking animation
        uint16_t offset = game->round->ticker % 30 >= 15? -3 : 3;
        vb_draw_img(buf, game_over_message, (buf.h_res - game_over_message.width) / 2 + offset, 80);
    
    } else if (game->state == ROUND_CORRECT_GUESS) {
        // Offset gives an effect of shaking animation
        uint16_t offset = game->round->ticker % 30 >= 15? -3 : 3;
        vb_draw_img(buf, correct_message, (buf.h_res - correct_message.width) / 2 + offset, 80);
    }

    if (game->round->role == DRAWER) {
        if (game_draw_buttons(game->round->attr.drawer) != OK)
            return 1;
    }

    return 0;
}

int game_give_clue() {
    if (game == NULL || game->round == NULL)
        return 1;

    size_t pos;
    if (word_clue_hint(&game->round->word_clue, &pos) != OK)
        return 0; // Not having more hints to give is not an error, but should return early.
    if (protocol_send_clue(pos) != OK)
        return 1;

    if (rtc_set_alarm_in(clue_time_interval) != OK)
        return 1;
    
    return 0;
}

int game_give_clue_at(size_t pos) {
    if (game == NULL || game->round == NULL)
        return 1;

    if (word_clue_hint_at(&game->round->word_clue, pos) != OK)
        return 1;
    
    return 0;
}

int game_round_over(uint32_t current_score, bool win) {
    game->score = current_score;
    clock_frames.current_frame = 1;
    
    if (win) {
        game->state = ROUND_CORRECT_GUESS;
    } else {
        game->state = GAME_OVER;
    }

    clue_reveal(&game->round->word_clue);

    if (game->round->role == DRAWER) {
        if (rtc_disable_int(ALARM_INTERRUPT) != OK)
            return 1;
    }

    if (game->round->role == GUESSER || !win) {
        if (rtc_set_alarm_in(end_round_delay) != OK)
            return 1;
    }

    return 0;
}

int game_guess_word(char *guess) {
    if (game == NULL || game->round == NULL)
        return 1;

    guess_t g;
    g.guess = guess;
    g.correct = strcmp(guess, game->round->correct_guess) == 0;
    if (game->round->num_guesses == MAX_GUESSES) {
        free(game->round->guesses[0].guess);
        for (int i = 1; i < MAX_GUESSES; i++) {
            game->round->guesses[i-1] = game->round->guesses[i];
        }
        game->round->guesses[MAX_GUESSES - 1] = g;
    } else {
        game->round->guesses[game->round->num_guesses] = g;
        game->round->num_guesses++;
    }

    if (g.correct) {
        game->state = ROUND_CORRECT_GUESS;
        if (game->round->role == DRAWER) {
            uint32_t new_score = game->score + 100 + game->round->round_timer * 4;
            if (new_score > MAX_SCORE)
                new_score = MAX_SCORE;
            if (event_round_win(new_score) != OK)
                return 1;
        }
    } else {
        game->round->round_timer -= TICKS_PER_SECOND * WRONG_GUESS_PENALTY;
        if (game->round->round_timer < 0) game->round->round_timer = 0;
    }

    return 0;
}

int game_rtc_alarm() {
    switch (game->state) {
        case ROUND_ONGOING:
            if (game->round->role == DRAWER) {
                if (game_give_clue() != OK)
                    return 1;
            }
            break;
        case GAME_OVER:
            if (rtc_disable_int(ALARM_INTERRUPT) != OK)
                return 1;
            if (event_end_round() != OK)
                return 1;
            if (menu_set_game_over_screen() != OK)
                return 1;
            break;
        case ROUND_CORRECT_GUESS:
            if (rtc_disable_int(ALARM_INTERRUPT) != OK)
                return 1;
            if (game->round->role == GUESSER) {
                if (event_end_round() != OK)
                    return 1;
                if (event_new_round_as_drawer() != OK)
                    return 1;
            }
            
            break;
        default:
            break;
    }
    return 0;
}

int game_other_player_game_over() {
    if (game == NULL || game->round == NULL)
        return 1;
    game->round->other_player_game_over = true;
    if (game->state == TIMES_UP) {
        if (game_round_over(game->score, false) != OK)
            return 1;
    }
    return 0;
}

int game_rtc_pi_tick() {
    if (game == NULL || game->round == NULL)
        return 1;

    if (game->round->role == GUESSER) {
        text_box_cursor_tick(&game->round->attr.guesser->text_box);
    }

    if (game->state == ROUND_ONGOING) {
        if (game->round->round_timer == 0) {
            game->state = TIMES_UP;
            clock_frames.current_frame = 1;
            if (protocol_send_game_over() != OK)
                return 1;
            if (game->round->other_player_game_over) {
                if (game_round_over(game->score, false) != OK)
                    return 1;
            }
        } else {
            game->round->round_timer--;
        }
    }
    
    return 0;
}

int game_timer_tick() {
    if (game == NULL || game->round == NULL)
        return 1;

    game->round->ticker++;

    if (game->state == ROUND_ONGOING) {
        if (game->round->ticker == 10) {
            clock_frames.current_frame = 0;
        } else if (game->round->ticker == 30) {
            clock_frames.current_frame = 1;
        } else if (game->round->ticker == 40) {
            clock_frames.current_frame = 2;
        } else if (game->round->ticker >= 60) {
            clock_frames.current_frame = 1;
            game->round->ticker = 0;
        }
    }

    return 0;
}

int drawer_change_selected_color() {
    if (game == NULL || game->round == NULL || game->round->role != DRAWER)
        return 1;
    drawer_t *drawer = game->round->attr.drawer;

    drawer->selected_color++;
    if (drawer->selected_color >= NUM_COLORS_AVAILABLE) {
        drawer->selected_color = 0;
    }
    button_set_circle_icon(&drawer->b_color, BUTTON_CIRCLE_RADIUS_DEFAULT, canvas_pallete[drawer->selected_color]);
    return 0;
}

int drawer_change_selected_thickness() {
    if (game == NULL || game->round == NULL || game->round->role != DRAWER)
        return 1;
    drawer_t *drawer = game->round->attr.drawer;

    drawer->selected_thickness++;
    if (drawer->selected_thickness >= NUM_THICKNESSES_AVAILABLE) {
        drawer->selected_thickness = 0;
    }
    button_set_circle_icon(&drawer->b_thickness, valid_thickness[drawer->selected_thickness], BUTTON_CIRCLE_DEFAULT_COLOR);
    return 0;
}

uint32_t drawer_get_selected_color() {
    drawer_t *drawer = game->round->attr.drawer;

    if (drawer->is_pencil_primary)
        return canvas_pallete[drawer->selected_color];
    else
        return 0x00FFFFFF;
}

uint16_t drawer_get_selected_thickness() {
    drawer_t *drawer = game->round->attr.drawer;
    
    return valid_thickness[drawer->selected_thickness];
}

int drawer_toggle_pencil_eraser() {
    if (game == NULL || game->round == NULL || game->round->role != DRAWER)
        return 1;
    drawer_t *drawer = game->round->attr.drawer;

    if (drawer->is_pencil_primary) {
        drawer_set_eraser_primary(drawer);
    } else {
        drawer_set_pencil_primary(drawer);
    }

    return 0;
}

int drawer_set_pencil_primary() {
    if (game == NULL || game->round == NULL || game->round->role != DRAWER)
        return 1;
    drawer_t *drawer = game->round->attr.drawer;

    drawer->is_pencil_primary = true;
    button_set_border_active(&drawer->b_pencil);
    button_unset_border_active(&drawer->b_eraser);
    return 0;
}

int drawer_set_eraser_primary() {
    if (game == NULL || game->round == NULL || game->round->role != DRAWER)
        return 1;
    drawer_t *drawer = game->round->attr.drawer;

    drawer->is_pencil_primary = false;
    button_set_border_active(&drawer->b_eraser);
    button_unset_border_active(&drawer->b_pencil);
    return 0;
}
