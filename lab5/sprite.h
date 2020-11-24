#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <lcom/lcf.h>

typedef struct {
    int x, y; // current position
    int xspeed, yspeed; // current speed
    xpm_image_t image; // the pixmap
} Sprite;

Sprite *create_sprite(xpm_map_t pic, int x, int y, int xspeed, int yspeed);
void destroy_sprite(Sprite *sp);
void animate_sprite(Sprite *sp);
int draw_sprite(Sprite *sp);

#endif /* _SPRITE_H_ */
