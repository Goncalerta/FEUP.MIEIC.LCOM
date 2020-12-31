#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <lcom/lcf.h>


typedef struct frame_buffer_t {
    void *buf;
    uint16_t h_res;	        /* Horizontal resolution in pixels */
    uint16_t v_res;	        /* Vertical resolution in pixels */
    size_t bytes_per_pixel; /* Number of VRAM bytes per pixel */
} frame_buffer_t;

typedef struct xpm_animation_t {
    uint16_t width, height;
    size_t number_of_frames;
    size_t current_frame;
    xpm_image_t *frames;
} xpm_animation_t;

int vb_draw_pixel(frame_buffer_t buf, uint16_t x, uint16_t y, uint32_t color);
int vb_fill_screen(frame_buffer_t buf, uint32_t color);
int vb_draw_hline(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int vb_draw_vline(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int vb_draw_line(frame_buffer_t buf, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color, uint16_t width);
int vb_draw_rectangle(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int vb_draw_circle(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t radius,  uint32_t color);
int vb_draw_img(frame_buffer_t buf, xpm_image_t img, uint16_t x, uint16_t y);
int vb_draw_img_cropped(frame_buffer_t buf, xpm_image_t img, uint16_t img_start_x, uint16_t img_start_y, uint16_t img_delta_x, uint16_t img_delta_y, uint16_t x, uint16_t y);

int vb_draw_animation_frame(frame_buffer_t buf, xpm_animation_t anim, uint16_t x, uint16_t y);
int xpm_load_animation(xpm_animation_t *anim, enum xpm_image_type type, size_t number_of_frames, ...);
void xpm_unload_animation(xpm_animation_t *anim);

#endif /* _GRAPHICS_H */
