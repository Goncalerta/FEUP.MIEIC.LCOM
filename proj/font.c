#include <lcom/lcf.h>
#include "font.h"
#include "pixmap_font.h"
#include "graphics.h"
#include "video_gr.h"

static xpm_image_t font;

int load_font(enum xpm_image_type type) {
    xpm_map_t xpm = font_xpm;
    if (xpm_load(xpm, type, &font) == NULL) {
        return 1;
    }
    return 0;
}

int draw_char(char c, uint16_t x, uint16_t y) {
    //just to check the size of the font
    vb_draw_img(vg_get_back_buffer(), font, 0, 0);
    vg_flip_page();
    return 0;

    //TODO
    // mapear char em coordenadas da font 
    // alterar int vb_draw_img() [de graphics.c] de modo a ser mais parametrizável (dar draw a partes da img)
    return 1;
}

int draw_string(char string[], uint16_t x, uint16_t y) {
    //TODO
    return 1;
}

//TODO cursor é de formato diferente, talvez implementar como sprite até para poder "piscar"

/*
load_font(XPM_8_8_8);
draw_char(0,0,0);
tickdelay(micros_to_ticks(SECONDS_TO_MICROS*10));
*/
