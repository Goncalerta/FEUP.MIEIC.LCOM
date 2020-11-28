#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <lcom/lcf.h>

#define SECONDS_TO_MICROS 1000000

#define VBE_LINEAR_FRAME_BUFFER_MODEL BIT(14)
#define VBE_CALL_AH 0x4F
#define VBE_FUNCTION_GET_CONTR_INFO 0x00
#define VBE_FUNCTION_RETURN_MODE_INFO 0x01
#define VBE_FUNCTION_SET_MODE 0x02
#define VBE_FUNCTION_SET_GET_DISPLAY_START 0x07
#define VBE_FUNCTION_AH_SUCCESS 0x00

#define BIOS_VIDEO_SERVICES 0x10

#define SET_DISPLAY_START_DURING_VERTICAL_RETRACE 0x80

#define MEMORY_MODEL_DIRECT_COLOR 0x06

#define COLOR_BYTE(val, n) ((val) >> (n*8))
#define COLOR_CAP_BYTES_NUM(n) (0xffffffff >> 2*(4-(n))) /* 1 <= n <= 4*/
#define COLOR_MASK(size, offset) ((0xffffffff >> (32-(size))) << (offset))

typedef struct video_buffer_t {
    void *buf;
    uint16_t h_res;	        /* Horizontal resolution in pixels */
    uint16_t v_res;	        /* Vertical resolution in pixels */
    size_t bytes_per_pixel; /* Number of VRAM bytes per pixel */
} video_buffer_t;

int vb_draw_pixel(video_buffer_t buf, uint16_t x, uint16_t y, uint32_t color);
int vb_fill_screen(video_buffer_t buf, uint32_t color);
int vb_draw_hline(video_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int vb_draw_vline(video_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int vb_draw_line(video_buffer_t buf, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color);
int vb_draw_rectangle(video_buffer_t buf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
// int vb_draw_pattern(video_buffer_t buf, uint8_t no_rectangles, uint32_t first, uint8_t step);
int vb_draw_circle(video_buffer_t buf, uint16_t x, uint16_t y, uint16_t radius,  uint32_t color);
int vb_draw_img(video_buffer_t buf, xpm_image_t img, uint16_t x, uint16_t y);

#endif /* _GRAPHICS_H */
