#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <lcom/lcf.h>

/** @file 
 * @brief File dedicated to graphics.
 */

/** @defgroup graphics graphics
 * @{
 *
 * @brief Module dedicated to graphics.
 */

/**
 * @brief Frame buffer info.
 * 
 */
typedef struct frame_buffer_t {
    void *buf; /*!< @brief Address of memory of the buffer. */
    uint16_t h_res;	/*!< @brief Horizontal resolution in pixels. */
    uint16_t v_res;	/*!< @brief Vertical resolution in pixels. */
    size_t bytes_per_pixel; /*!< @brief Number of VRAM bytes per pixel. */
} frame_buffer_t;

/**
 * @brief xpm animation info.
 * 
 */
typedef struct xpm_animation_t {
    uint16_t width; /*!< @brief Animation width. */
    uint16_t height; /*!< @brief Animation height. */
    size_t number_of_frames; /*!< @brief Animation number of fraames. */
    size_t current_frame; /*!< @brief Animation current frame. */
    xpm_image_t *frames; /*!< @brief Address of memory of the animation frames. */
} xpm_animation_t;

/**
 * @brief Draws a pixel in the given buffer in the given coordinates with the given color.
 * 
 * @param buf buffer
 * @param x x coordinate
 * @param y y coordinate
 * @param color color
 * @return Return 0 upon success and non-zero otherwise
 */
int vb_draw_pixel(frame_buffer_t buf, uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief Fills the given buffer with the given color.
 * 
 * @param buf buffer
 * @param color color
 * @return Return 0 upon success and non-zero otherwise
 */
int vb_fill_screen(frame_buffer_t buf, uint32_t color);

/**
 * @brief Draws a horizontal line in the given buffer in the given coordinates with the given color.
 * 
 * @param buf buffer
 * @param x left most x coordinate
 * @param y y coordinate
 * @param len line length
 * @param color line color
 * @return Return 0 upon success and non-zero otherwise
 */
int vb_draw_hline(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * @brief Draws a vertical line in the given buffer in the given coordinates with the given color.
 * 
 * @param buf buffer
 * @param x x coordinate
 * @param y top most y coordinate
 * @param len line length
 * @param color line color
 * @return Return 0 upon success and non-zero otherwise
 */
int vb_draw_vline(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * @brief Draws a line in the given buffer from the given coordinates to other given coordinates with the given width and color.
 * 
 * @param buf buffer
 * @param x1 start x coordinate
 * @param y1 start y coordinate
 * @param x2 end x coordinate
 * @param y2 end y coordinate
 * @param color line color
 * @param width line width
 * @return Return 0 upon success and non-zero otherwise
 */
int vb_draw_line(frame_buffer_t buf, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color, uint16_t width);

/**
 * @brief Draws a rectangle in the given buffer in the given coordinates with the given height and width with the given color.
 * 
 * @param buf buffer
 * @param x rectangle left most x coordinate
 * @param y rectangle top most y coordinate
 * @param width rectange width
 * @param height rectangle height
 * @param color rectangle color
 * @return Return 0 upon success and non-zero otherwise
 */
int vb_draw_rectangle(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

/**
 * @brief Draws a circle in the given buffer in the given coordinates with the given radius with the given color.
 * 
 * @param buf buffer
 * @param x circle center x coordinate
 * @param y circle center y coordinate
 * @param radius circle radius
 * @param color circle color
 * @return Return 0 upon success and non-zero otherwise
 */
int vb_draw_circle(frame_buffer_t buf, uint16_t x, uint16_t y, uint16_t radius,  uint32_t color);

/**
 * @brief Draws a given xpm image in the given buffer in the given coordinates.
 * 
 * @param buf buffer
 * @param img xpm image
 * @param x left most x coordinate
 * @param y top most y coordinate
 * @return Return 0 upon success and non-zero otherwise
 */
int vb_draw_img(frame_buffer_t buf, xpm_image_t img, uint16_t x, uint16_t y);

/**
 * @brief Draws a portion of a given xpm image in the given buffer in the given coordinates.
 * 
 * @param buf buffer
 * @param img xpm image
 * @param img_start_x start image x coordinate to draw
 * @param img_start_y start image y coordinate to draw
 * @param img_delta_x image width portion to draw
 * @param img_delta_y image height portion to draw
 * @param x buffer x coordiante
 * @param y buffer y coordinate
 * @return Return 0 upon success and non-zero otherwise 
 */
int vb_draw_img_cropped(frame_buffer_t buf, xpm_image_t img, uint16_t img_start_x, uint16_t img_start_y, uint16_t img_delta_x, uint16_t img_delta_y, uint16_t x, uint16_t y);

/**
 * @brief Draws a given animation in the given buffer in the given coordinates.
 * 
 * @param buf buffer
 * @param anim animation
 * @param x x coordinate
 * @param y y coordinate
 * @return Return 0 upon success and non-zero otherwise
 */
int vb_draw_animation_frame(frame_buffer_t buf, xpm_animation_t anim, uint16_t x, uint16_t y);

/**
 * @brief Loads an animation witdh the given frames.
 * 
 * @param anim address of memory of the animation
 * @param type type of xpm image
 * @param number_of_frames animation number of frames
 * @param ... animation frames
 * @return Return 0 upon success and non-zero otherwise
 */
int xpm_load_animation(xpm_animation_t *anim, enum xpm_image_type type, size_t number_of_frames, ...);

/**
 * @brief Frees the space allocated in memory to store the image frames of a given animation.
 * 
 * @param anim address of memory of the animation
 */
void xpm_unload_animation(xpm_animation_t *anim);

/**@}*/

#endif /* _GRAPHICS_H */
