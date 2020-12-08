#include <lcom/lcf.h>
#include "button.h"

#define BUTTON_BORDER_COLOR 0x00aaaaaa
#define BUTTON_BORDER_ACTIVE_COLOR 0x0043A047
#define BUTTON_FILL_NORMAL_COLOR 0x00dddddd
#define BUTTON_FILL_HOVERING_COLOR 0x00eeeeee
#define BUTTON_FILL_PRESSING_COLOR 0x00999999
#define BUTTON_MARGIN 3

int new_button(button_t *button, uint16_t x, uint16_t y, uint16_t width, uint16_t height, button_action action) {
    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    button->action = action;
    button->state = BUTTON_NORMAL;
    button->icon.type = BUTTON_ICON_NONE;
    button->active_border = false;

    return 0;
}

void button_set_border_active(button_t *button) {
    button->active_border = true;
}

void button_unset_border_active(button_t *button) {
    button->active_border = false;
}

void button_set_xpm_icon(button_t *button, xpm_image_t icon) {
    button->icon.type = BUTTON_ICON_XPM;
    button->icon.attributes.img = icon;
}

void button_set_circle_icon(button_t *button, uint16_t radius, uint32_t color) {
    button->icon.type = BUTTON_ICON_CIRCLE;
    button->icon.attributes.circle.radius = radius;
    button->icon.attributes.circle.color = color;
}

bool button_is_hovering(button_t button, uint16_t x, uint16_t y) {
    return x >= button.x && y >= button.y 
        && x <= button.x + button.width && y <= button.y + button.height;
}

int button_draw(frame_buffer_t buf, button_t button) {
    uint32_t border_color = button.active_border? BUTTON_BORDER_ACTIVE_COLOR : BUTTON_BORDER_COLOR;
    uint32_t fill_color;
    switch (button.state) {
    case BUTTON_NORMAL:
    case BUTTON_PRESSING_NOT_HOVERING:
        fill_color = BUTTON_FILL_NORMAL_COLOR;
        break;
    case BUTTON_HOVERING:
        fill_color = BUTTON_FILL_HOVERING_COLOR;
        break;
    case BUTTON_PRESSING:
        fill_color = BUTTON_FILL_PRESSING_COLOR;
        break;
    }

    if (vb_draw_rectangle(buf, button.x, button.y, button.width, button.height, border_color))
        return 1;
    if (vb_draw_rectangle(buf, button.x + BUTTON_MARGIN, button.y + BUTTON_MARGIN, 
                          button.width - 2*BUTTON_MARGIN, button.height - 2*BUTTON_MARGIN, fill_color))
        return 1;

    uint16_t x, y;
    switch (button.icon.type) {
    case BUTTON_ICON_XPM:
        x = button.x + (button.width - button.icon.attributes.img.width) / 2;
        y = button.y + (button.height - button.icon.attributes.img.height) / 2;
        if (vb_draw_img(buf, button.icon.attributes.img, 0, 0, button.icon.attributes.img.width, button.icon.attributes.img.height, x, y) != OK)
            return 1;
        break;
    case BUTTON_ICON_CIRCLE:
        x = button.x + button.width/2;
        y = button.y + button.height/2;
        if (vb_draw_circle(buf, x, y, button.icon.attributes.circle.radius, button.icon.attributes.circle.color) != OK)
            return 1;
        break;
    default:
        break;
    } 

    return 0;
}

int button_update_state(button_t *button, bool hovering, bool lb, bool rb) {
    switch (button->state) {
    case BUTTON_NORMAL:
        if (hovering && !(lb || rb)) {
            button->state = BUTTON_HOVERING;
        }
        break;
    
    case BUTTON_HOVERING:
        if (hovering) {
            if (lb && !rb) {
                button->state = BUTTON_PRESSING;
            }
        } else {
            button->state = BUTTON_NORMAL;
        }
        break;

    case BUTTON_PRESSING:
        if (rb) {
            button->state = hovering? BUTTON_HOVERING : BUTTON_NORMAL;
        } else if (!lb) {
            if (hovering) {
                button->state = BUTTON_HOVERING;
                if (button->action() != OK)
                    return 1; 
            } else {
                button->state = BUTTON_NORMAL; 
            }
        } else if (!hovering) {
            button->state = BUTTON_PRESSING_NOT_HOVERING; 
        }
        break;
    
    case BUTTON_PRESSING_NOT_HOVERING:
        if (!lb || rb) {
            button->state = BUTTON_NORMAL;
        } else if (hovering) {
            button->state = BUTTON_PRESSING;
        }
        break;
    }

    return 0;
}


// bool is_hovering_tb(triangle_button sb, uint16_t mouse_x, uint16_t mouse_y) {
//     return mouse_x >= sb.x && mouse_x <= sb.x + sb.width
//         && mouse_y >= sb.y + sb.height * (0.5 + (mouse_x - sb.x) / (2 * sb.width))
//         && mouse_y <= sb.y + (sb.y + sb.height) * (mouse_x - sb.x - sb.width) / (2 * sb.width);
// }

// int tb_draw(triangle_button tb) {
//     switch (tb.state) {
//     case BUTTON_NORMAL:
//         // TODO
//         // draw_img(tb_normal, tb.x, tb.y);
//         break;
//     case BUTTON_HOVERING:
//         // TODO
//         // draw_img(tb_hovering, tb.x, tb.y);
//         break;
//     case BUTTON_PRESSING:
//         // TODO
//         // draw_img(tb_pressing, tb.x, tb.y);
//         break;
//     }

//     // TODO
//     // draw_img(tb.icon, tb.x, tb.y);
// }
