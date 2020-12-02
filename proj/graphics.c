#include <lcom/lcf.h>
#include <math.h>
#include "graphics.h"

// static uint8_t red_mask_size;
// static uint8_t green_mask_size;
// static uint8_t blue_mask_size;
// static bool direct_color_mode;

int vb_draw_pixel(frame_buffer_t buf, uint16_t x, uint16_t y, uint32_t color) {
    if ((x >= buf.h_res) || (y >= buf.v_res)) {
        // TODO rectangles outside borders are failing because of this. maybe just not paint the pixel instead of error?
        //      another option is to check in draw functions before drawing pixel
        //printf("Error trying to print outside the screen limits.\n"); 
        return 0;
    }

    uint8_t *pixel_mem_pos = (uint8_t*) buf.buf + (y * buf.h_res * buf.bytes_per_pixel) + (x * buf.bytes_per_pixel);

    if (color > COLOR_CAP_BYTES_NUM(buf.bytes_per_pixel)) {
        printf("Invlid color argument. Too large\n");
        return 1;
    }
    for (uint8_t i = 0; i < buf.bytes_per_pixel; i++) {
        *(pixel_mem_pos+i) = (uint8_t) COLOR_BYTE(color, i);
    }
    return 0;
}

int vb_fill_screen(frame_buffer_t buf, uint32_t color) {
    uint8_t *pixel_mem_pos = (uint8_t*) buf.buf;

    if (color > COLOR_CAP_BYTES_NUM(buf.bytes_per_pixel)) {
        printf("Invlid color argument. Too large\n");
        return 1;
    }

    for (size_t px = 0; px < buf.h_res * buf.v_res * buf.bytes_per_pixel; px += buf.bytes_per_pixel) {
        for (uint8_t i = 0; i < buf.bytes_per_pixel; i++) {
            pixel_mem_pos[px + i] = (uint8_t) COLOR_BYTE(color, i);
        }
    }
    
    return 0;
}

int vb_draw_hline(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        if (vb_draw_pixel(buf, x + i, y, color)) {
            return 1;
        }
    }
    return 0;
}

int vb_draw_vline(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
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

// static void vg_parse_color(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue) {
//     *blue = color & COLOR_MASK(blue_mask_size, 0);
//     *green = color & COLOR_MASK(green_mask_size, blue_mask_size);
//     *red = color & COLOR_MASK(red_mask_size, blue_mask_size + green_mask_size);
// }

// static uint32_t vg_create_color(uint8_t red, uint8_t green, uint8_t blue) {
//     return blue | (green << blue_mask_size) | (red << (blue_mask_size + green_mask_size));
// }

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
    // TODO is this optimization worth it?
    // if (y1 == y2) {
    //     if (x2 < x1) {
    //         int16_t temp = x2;
    //         x2 = x1;
    //         x1 = temp;
    //     }
    //     for (uint16_t x = x1; x <= x2; x++) {
    //         if (vb_draw_circle(buf, x, y1, width, color) != OK) {
    //             return 1;
    //         }
    //         // if (vb_draw_pixel(buf, x, y1, color) != OK) {
    //         //     return 1;
    //         // }
    //     }
    //     return 0;
    // }
    // if (x1 == x2) {
    //     if (y2 < y1) {
    //         int16_t temp = y2;
    //         y2 = y1;
    //         y1 = temp;
    //     }
    //     for (uint16_t y = y1; y <= y2; y++) {
    //         if (vb_draw_circle(buf, x1, y, width, color) != OK) {
    //             return 1;
    //         }
    //         // if (vb_draw_pixel(buf, x1, y, color) != OK) {
    //         //     return 1;
    //         // }
    //     }
    //     return 0;
    // }

    int dx = abs(x2 - x1); 
    int sx = x1 < x2? 1 : -1;
    int dy = abs(y2 - y1); 
    int sy = y1 < y2? 1 : -1; 
    int err = dx - dy;
 
    while (x1 != x2 || y1 != y2) {
        if (vb_draw_circle(buf, x1, y1, width, color) != OK) {
            return 1;
        }
        // if (vb_draw_pixel(buf, x1, y1, color) != OK) {
        //     return 1;
        // }
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

// int vg_draw_pattern(frame_buffer_t buf, uint8_t no_rectangles, uint32_t first, uint8_t step) {
//     uint16_t width = h_res/no_rectangles;
//     uint16_t height = v_res/no_rectangles;

//     uint8_t first_red, first_green, first_blue;
//     if (direct_color_mode) {
//         vg_parse_color(first, &first_red, &first_green, &first_blue);
//     }

//     for (uint16_t row = 0; row < no_rectangles; row++) {
//         for (uint16_t col = 0; col < no_rectangles; col++) {
//             uint32_t color;
//             if (direct_color_mode) {
//                 uint8_t red = (first_red + col * step) % (1 << red_mask_size);
//                 uint8_t green = (first_green + row * step) % (1 << green_mask_size);
//                 uint8_t blue = (first_blue + (col + row) * step) % (1 << blue_mask_size);
//                 color = vg_create_color(red, green, blue);
//             } else {
//                 color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
//             }

//             if (vg_draw_rectangle(buf, col * width, row * height, width, height, color) != OK)
//                 return 1;
//         }
//     }

//     return 0;
// }

// int vb_draw_img(frame_buffer_t buf, xpm_image_t img, uint16_t x, uint16_t y) {
//     for (uint16_t i = 0; i < img.width; i++) {
//         for (uint16_t j = 0; j < img.height; j++) {
//             uint32_t color = 0;
//             for (uint8_t k = 0; k < buf.bytes_per_pixel; k++) {
//                 color += img.bytes[(i + j * img.width) * buf.bytes_per_pixel] << (8 * k);
//             }

//             if (color == xpm_transparency_color(img.type)) 
//                 continue;

//             if (vb_draw_pixel(buf, x + i, y + j, color) != OK)
//                 return 1;
//         }
//     }
    
//     return 0;
// }

/* to draw a whole image pass: img_start_x = 0; img_start_y = 0; img_delta_x = img.width; img_delta_y = img.height */
int vb_draw_img(frame_buffer_t buf, xpm_image_t img, uint16_t img_start_x, uint16_t img_start_y, uint16_t img_delta_x, uint16_t img_delta_y, uint16_t x, uint16_t y) {
    if	((img_start_x + img_delta_x > img.width) || (img_start_y + img_delta_y > img.height)) {
        printf("Invalid region of image to draw (out of bounds).\n");
        printf("%d %d %d        %d %d %d\n", img_start_x, img_delta_x, img.width, img_start_y, img_delta_y, img.height);
        return 1;
    }

    for (uint16_t j = 0; j < img_delta_y; j++) {
        for (uint16_t i = 0; i < img_delta_x; i++) {
            uint32_t color = 0;
            for (uint8_t k = 0; k < buf.bytes_per_pixel; k++) {
                color += img.bytes[(i + img_start_x + (j+img_start_y) * img.width) * buf.bytes_per_pixel + k] << (8 * k);
            }

            if (color == xpm_transparency_color(img.type)) 
                continue;

            if (vb_draw_pixel(buf, x + i, y + j, color) != OK)
                return 1;
        }
    }
    
    return 0;
}
