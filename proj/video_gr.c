#include <lcom/lcf.h>
#include <math.h>

#include "video_gr.h"
#include "vbe.h"
#include "graphics.h"

static void *video_buf1;         /* frame-buffer VM address */
static void *video_buf2;
static bool buf1_is_primary = true;
static uint16_t h_res;	        /* Horizontal resolution in pixels */
static uint16_t v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static uint8_t red_mask_size;
static uint8_t green_mask_size;
static uint8_t blue_mask_size;
static bool direct_color_mode;

static void *vg_get_back_buffer() {
    if (buf1_is_primary) {
        return video_buf2;
    } else {
        return video_buf1;
    }
}

static void vg_set_global_var_screen(vbe_mode_info_t *vmi) {
    h_res = vmi->XResolution;
    v_res = vmi->YResolution;
    bits_per_pixel = vmi->BitsPerPixel;
    red_mask_size = vmi->RedMaskSize;
    green_mask_size = vmi->RedMaskSize;
    blue_mask_size = vmi->RedMaskSize;
    direct_color_mode = vmi->MemoryModel == MEMORY_MODEL_DIRECT_COLOR;
}

void *(vg_init)(uint16_t mode) {
    struct minix_mem_range mr1;
    struct minix_mem_range mr2;
    unsigned int vram_base, vram_size;

    /* Use VBE function 0x01 to initialize vram_base and vram_size */ 
    vbe_mode_info_t vmi;
    if (vbe_get_mode_inf(mode, &vmi)) {
        printf("Error while getting mode.\n");
        return NULL;
    }

    vg_set_global_var_screen(&vmi);

    vram_base = vmi.PhysBasePtr;
    vram_size = h_res * v_res * ceil(bits_per_pixel/8.0); // TODO shouldn't bits_per_pixel be a multiple of 8 already?


    /* Allow memory mapping */
    mr1.mr_base = vram_base;
    mr1.mr_limit = mr1.mr_base + vram_size;
    mr2.mr_base = mr1.mr_limit;
    mr2.mr_limit = mr2.mr_base + vram_size;

    int r;
    if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr1)) != OK)
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr2)) != OK)
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);

    /* Map memory */
    video_buf1 = vm_map_phys(SELF, (void *) mr1.mr_base, vram_size);
    if (video_buf1 == MAP_FAILED)
        panic("couldn't map video memory");

    video_buf2 = vm_map_phys(SELF, (void *) mr2.mr_base, vram_size);
    if (video_buf2 == MAP_FAILED)
        panic("couldn't map video memory");

    if (vbe_change_mode(mode)) {
        printf("Error while changing mode.\n");
        return NULL;
    }

    return video_buf1;
}

uint16_t vg_get_hres() {
    return h_res;
}

uint16_t vg_get_vres() {
    return v_res;
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
    if ((x >= h_res) || (y > v_res)) {
        // TODO rectangles outside borders are failing because of this. maybe just not paint the pixel instead of error?
        //      another option is to check in draw functions before drawing pixel
        //printf("Error trying to print outside the screen limits.\n"); 
        return 0;
    }
    uint8_t bytes_per_pixel = ceil(bits_per_pixel/8.0);

    uint8_t *pixel_mem_pos = (uint8_t*) vg_get_back_buffer() + (y * h_res * bytes_per_pixel) + (x * bytes_per_pixel);

    if (color > COLOR_CAP_BYTES_NUM(bytes_per_pixel)) {
        printf("Invlid color argument. Too large\n");
        return 1;
    }
    for (uint8_t i = 0; i < bytes_per_pixel; i++) {
        *(pixel_mem_pos+i) = (uint8_t) COLOR_BYTE(color, i);
    }
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        if (vg_draw_pixel(x + i, y, color)) {
            return 1;
        }
    }
    return 0;
}

int (vg_draw_vline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        if (vg_draw_pixel(x, y + i, color)) {
            return 1;
        }
    }
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t i = 0; i < height; i++) {
        if(vg_draw_hline(x, y+i, width, color))
            return 1;
    }

    return 0;
}

static void vg_parse_color(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue) {
    *blue = color & COLOR_MASK(blue_mask_size, 0);
    *green = color & COLOR_MASK(green_mask_size, blue_mask_size);
    *red = color & COLOR_MASK(red_mask_size, blue_mask_size + green_mask_size);
}

static uint32_t vg_create_color(uint8_t red, uint8_t green, uint8_t blue) {
    return blue | (green << blue_mask_size) | (red << (blue_mask_size + green_mask_size));
}

int vg_draw_circle(uint16_t x, uint16_t y, uint16_t radius,  uint32_t color) {
    int32_t top_left_x = x - radius;
    int32_t top_left_y = y - radius;
    int32_t max_distance = radius * radius;
    int32_t x32 = x, y32 = y; // So that calculations don't overflow
    
    for (int32_t i = top_left_x; i <= top_left_x + 2*radius; i++) {
        for (int32_t j = top_left_y; j <= top_left_y + 2*radius; j++) {
            if ((x32-i) * (x32-i) + (y32-j) * (y32-j) <= max_distance) {
                if (vg_draw_pixel(i, j, color)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int vg_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color) {

    // for (int16_t x = x1; x <= x2; x++) {
    //    int16_t y = (x - x1) * (y2 - y1) / (x2 - x1) + y1;
    //    if (vg_draw_pixel(x, y, color) != OK)
    //        return 1;
    // }

    // return 0;
}

int vg_draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step) {
    uint16_t width = h_res/no_rectangles;
    uint16_t height = v_res/no_rectangles;

    uint8_t first_red, first_green, first_blue;
    if (direct_color_mode) {
        vg_parse_color(first, &first_red, &first_green, &first_blue);
    }

    for (uint16_t row = 0; row < no_rectangles; row++) {
        for (uint16_t col = 0; col < no_rectangles; col++) {
            uint32_t color;
            if (direct_color_mode) {
                uint8_t red = (first_red + col * step) % (1 << red_mask_size);
                uint8_t green = (first_green + row * step) % (1 << green_mask_size);
                uint8_t blue = (first_blue + (col + row) * step) % (1 << blue_mask_size);
                color = vg_create_color(red, green, blue);
            } else {
                color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
            }

            if (vg_draw_rectangle(col * width, row * height, width, height, color) != OK)
                return 1;
        }
    }

    return 0;
}

int vg_draw_img(xpm_image_t img, uint16_t x, uint16_t y) {
    for (uint16_t i = 0; i < img.width; i++) {
        for (uint16_t j = 0; j < img.height; j++) {
            uint8_t bytes_per_pixel = ceil(bits_per_pixel/8.0);
            uint32_t color = 0;
            for (uint8_t k = 0; k < bytes_per_pixel; k++) {
                color += img.bytes[(i + j * img.width) * bytes_per_pixel] << (8 * k);
            }

            if (color == xpm_transparency_color(img.type)) 
                continue;

            if (vg_draw_pixel(x + i, y + j, color) != OK)
                return 1;
        }
    }
    
    return 0;
}

int vg_flip_page() {
    if (buf1_is_primary) {
        if (vbe_set_display_start(0, v_res))
            return 1;
    } else {
        if (vbe_set_display_start(0, 0))
            return 1;
    }
    buf1_is_primary = !buf1_is_primary;
    return 0;
}

int vg_clear() {
    for (int i = 0; i < ceil(bits_per_pixel * h_res * v_res / 8.0); i++) {
        ((uint8_t *) vg_get_back_buffer())[i] = 0; // TODO maybe setmem or something like that may be more efficient?
    }
    return 0;
}
