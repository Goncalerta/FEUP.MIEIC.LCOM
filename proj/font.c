#include <lcom/lcf.h>
#include "font.h"
#include "pixmap_font.h"
#include "graphics.h"

// static xpm_image_t font;

int load_font() {
    //TODO
    // xpm_map_t xpm = ???
    // if (xpm_load(xpm, ???, &font) == NULL) {
    //   return 1;
    // }
    // return 0;
    return 1;
}

int draw_char(char c, uint16_t x, uint16_t y) {
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
