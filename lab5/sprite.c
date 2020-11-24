#include "sprite.h"
#include "video_gr.h"

/** Creates a new sprite with pixmap "pic", with specified
* position (within the screen limits) and speed;
* Does not draw the sprite on the screen
* Returns NULL on invalid pixmap.
*/
Sprite *create_sprite(xpm_map_t pic, int x, int y, int xspeed, int yspeed) {
    //allocate space for the "object"
    Sprite *sp = (Sprite *) malloc ( sizeof(Sprite) );
    if( sp == NULL )
        return NULL;
    // read the sprite pixmap
    if( xpm_load(pic, XPM_INDEXED, &sp->image) == NULL ) {
        free(sp);
        return NULL;
    }

    sp->x = x;
    sp->y = y;
    sp->xspeed = xspeed;
    sp->yspeed = yspeed;
    return sp;
}

void destroy_sprite(Sprite *sp) {
    if( sp == NULL )
        return;
    if( sp->image.bytes )
        free(sp->image.bytes);
    free(sp);
    sp = NULL;
}

void animate_sprite(Sprite *sp) {
    sp->x += sp->xspeed;
    sp->y += sp->yspeed;
}

int draw_sprite(Sprite *sp) {
    return vg_draw_img(sp->image, sp->x, sp->y);
}
