#include <lcom/lcf.h>
#include <math.h>
#include <stdarg.h>
#include "graphics.h"

#define COLOR_CAP_BYTES_NUM(n) (0xffffffff >> 8*(4-(n))) /* 1 <= n <= 4*/

#define COLOR_BYTE(val, n) ((val) >> (n*8)) // TODO not being used, delete?
#define COLOR_MASK(size, offset) ((0xffffffff >> (32-(size))) << (offset)) // TODO not being used, delete?

int vb_draw_pixel(frame_buffer_t buf, uint16_t x, uint16_t y, uint32_t color) {
    if ((x >= buf.h_res) || (y >= buf.v_res))
        return 0;

    uint8_t *pixel_mem_pos = (uint8_t*) buf.buf + (y * buf.h_res * buf.bytes_per_pixel) + (x * buf.bytes_per_pixel);

    if (color > COLOR_CAP_BYTES_NUM(buf.bytes_per_pixel)) {
        printf("Invlid color argument. Too large\n");
        return 1;
    }

    if (memcpy(pixel_mem_pos, &color, buf.bytes_per_pixel) == NULL) {
        printf("Error painting.\n");
        return 1;
    }
    return 0;
}

int vb_fill_screen(frame_buffer_t buf, uint32_t color) {
    // This algorithm fixed the slow mouse problem in our computer
    // memcpys probably are more optimized than for loops pixel by pixel
    uint8_t *begin = buf.buf;
    size_t half_len = buf.v_res * buf.h_res * buf.bytes_per_pixel/2;

    memcpy(begin, &color, buf.bytes_per_pixel);

    for (size_t size = buf.bytes_per_pixel; size < half_len; size *= 2) {
        if (memcpy(begin + size, begin, size) == NULL)
            return 1;
    }

    memcpy(begin + half_len, begin, half_len);
    
    return 0;
}

int vb_draw_hline(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        // TODO for efficiency, maybe copy all bytes at once, checking the color and out of screen only one
        if (vb_draw_pixel(buf, x + i, y, color)) {
            return 1;
        }
    }
    return 0;
}

int vb_draw_vline(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        // TODO for efficiency, maybe copy all bytes at once, checking the color and out of screen only one
        if (vb_draw_pixel(buf, x, y + i, color)) {
            return 1;
        }
    }
    return 0;
}

int vb_draw_rectangle(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t i = 0; i < height; i++) {
        if(vb_draw_hline(buf, x, y+i, width, color))
            return 1;
    }

    return 0;
}

int vb_draw_circle(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t radius,  uint32_t color) {
    int32_t top_left_x = x - radius;
    int32_t top_left_y = y - radius;
    int32_t max_distance = radius * radius;
    int32_t x32 = x, y32 = y; // So that calculations don't overflow
    
    for (int32_t i = top_left_x; i <= top_left_x + 2*radius; i++) {
        for (int32_t j = top_left_y; j <= top_left_y + 2*radius; j++) {
            if ((x32-i) * (x32-i) + (y32-j) * (y32-j) <= max_distance) {
                if (vb_draw_pixel(buf, i, j, color)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int vb_draw_line(frame_buffer_t buf, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color, uint16_t width) {
    int dx = abs(x2 - x1); 
    int sx = x1 < x2? 1 : -1;
    int dy = abs(y2 - y1); 
    int sy = y1 < y2? 1 : -1; 
    int err = dx - dy;
 
    while (x1 != x2 || y1 != y2) {
        if (vb_draw_circle(buf, x1, y1, width, color) != OK) {
            return 1;
        }

        int e2 = 2*err;
        if (e2 >= -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }

    return 0;
}

int vb_draw_img(frame_buffer_t buf, xpm_image_t img, uint16_t x, uint16_t y) {
    return vb_draw_img_cropped(buf, img, 0, 0, img.width, img.height, x, y);
}

int vb_draw_img_cropped(frame_buffer_t buf, xpm_image_t img, uint16_t img_start_x, uint16_t img_start_y, uint16_t img_delta_x, uint16_t img_delta_y, uint16_t x, uint16_t y) {
    if	((img_start_x + img_delta_x > img.width) || (img_start_y + img_delta_y > img.height)) {
        printf("Invalid region of image to draw (out of bounds).\n");
        printf("%d %d %d        %d %d %d\n", img_start_x, img_delta_x, img.width, img_start_y, img_delta_y, img.height);
        return 1;
    }

    for (uint16_t j = 0; j < img_delta_y; j++) {
        for (uint16_t i = 0; i < img_delta_x; i++) {
            uint32_t color = 0;
            for (uint8_t k = 0; k < buf.bytes_per_pixel; k++) {
                color += img.bytes[(i + img_start_x + (j + img_start_y) * img.width) * buf.bytes_per_pixel + k] << (8 * k);
            }

            if (color == xpm_transparency_color(img.type)) 
                continue;

            if (vb_draw_pixel(buf, x + i, y + j, color) != OK)
                return 1;
        }
    }
    
    return 0;
}

int vb_draw_animation_frame(frame_buffer_t buf, xpm_animation_t anim, uint16_t x, uint16_t y) {
    return vb_draw_img(buf, anim.frames[anim.current_frame], x, y);
}

int xpm_load_animation(xpm_animation_t *anim, enum xpm_image_type type, size_t number_of_frames, ...) {
    anim->number_of_frames = number_of_frames;
    anim->current_frame = 0;
    anim->frames = malloc(number_of_frames * sizeof(xpm_image_t));
    if (anim->frames == NULL)
        return 1;

    xpm_image_t img;

    va_list ap;
    va_start(ap, number_of_frames);
    for (size_t i = 0; i < number_of_frames; i++) {
        xpm_map_t frame = va_arg(ap, xpm_map_t);
        if (xpm_load(frame, type, &img) == NULL) {
            va_end(ap);
            return 1;
        }
        
        anim->frames[i] = img;
    }
    va_end(ap);

    if (number_of_frames == 0) 
        return 1;
    
    anim->width = anim->frames[0].width;
    anim->height = anim->frames[0].height;

    return 0;
}

void xpm_unload_animation(xpm_animation_t *anim) {
    for (size_t i = 0; i < anim->number_of_frames; i++) {
        free(anim->frames[i].bytes);
    }
    free(anim->frames);
}
