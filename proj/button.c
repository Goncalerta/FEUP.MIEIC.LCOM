#include <lcom/lcf.h>
#include "button.h"

/** @defgroup button button
 * @{
 *
 */

#define BUTTON_BORDER_COLOR 0x00aaaaaa /**< @brief Normal color of the border of the button */
#define BUTTON_BORDER_ACTIVE_COLOR 0x0043A047 /**< @brief Color of the border of the button when it is active */
#define BUTTON_FILL_NORMAL_COLOR 0x00dddddd /**< @brief Normal color of the button */
#define BUTTON_FILL_HOVERING_COLOR 0x00eeeeee /**< @brief Color of the button when hovering */
#define BUTTON_FILL_PRESSING_COLOR 0x00999999 /**< @brief Color of the button when pressing */
#define BUTTON_MARGIN 3 /**< @brief Margin for the button border */

/**
 * @brief Enumerated type for specifying the state of a button.
 * 
 */
typedef enum button_state {
    BUTTON_NORMAL, /*!< Button is in normal/base state. */
    BUTTON_HOVERING, /*!< Button is not being pressed but the cursor is hovering it. */
    BUTTON_PRESSING, /*!< Button is being pressed and the cursor is hovering it. */
    BUTTON_PRESSING_NOT_HOVERING /*!< Button is being pressed but the cursor is not hovering it. */
} button_state_t;

/**
 * @brief Icon of a button.
 * 
 */
typedef struct button_icon {
    enum { 
        BUTTON_ICON_NONE, /*!< @brief No icon. */
        BUTTON_ICON_XPM, /*!< @brief xpm image icon. */
        BUTTON_ICON_CIRCLE /*!< @brief Circle icon. */
    } type; /*!< @brief Icon type. */

    union {
        xpm_image_t img; /*!< @brief Button image icon. */
        
        struct {
            uint16_t radius; /*!< @brief Radius of a circle icon. */
            uint32_t color; /*!< @brief Color of a circle icon. */
        } circle; /*!< @brief Button circle icon. */
    } attributes; /*!< @brief Icon attributes. */
} button_icon_t;

/**
 * @brief // Button class implementation.
 * 
 */
struct button {
    uint16_t x; /*!< @brief Left most x coordinate of the button. */
    uint16_t y; /*!< @brief Top most y coordinate of the button. */
    uint16_t width; /*!< @brief Button width. */
    uint16_t height; /*!< @brief Button height. */
    button_state_t state; /*!< @brief State of the button. */
    button_action_t action; /*!< @brief Action to perform when button is pressed. */
    button_icon_t icon; /*!< @brief Button icon. */
    bool active_border; /*!< @brief True if the border of the button is active and false otherwise. */
};

button_t *new_button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, button_action_t action) {
    button_t *button = malloc(sizeof(button_t));
    if (button == NULL)
        return NULL;
        
    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    button->action = action;
    button->state = BUTTON_NORMAL;
    button->icon.type = BUTTON_ICON_NONE;
    button->active_border = false;

    return button;
}

void delete_button(button_t *button) {
    if (button != NULL) {
        free(button);
    }
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

bool button_is_hovering(button_t *button, uint16_t x, uint16_t y) {
    return x >= button->x && y >= button->y 
        && x <= button->x + button->width && y <= button->y + button->height;
}

int button_draw(frame_buffer_t buf, button_t *button) {
    // Get border and fill color
    uint32_t border_color = button->active_border? BUTTON_BORDER_ACTIVE_COLOR : BUTTON_BORDER_COLOR;
    uint32_t fill_color;
    switch (button->state) {
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

    // Draw button and border
    if (vb_draw_rectangle(buf, button->x, button->y, button->width, button->height, border_color))
        return 1;
    if (vb_draw_rectangle(buf, button->x + BUTTON_MARGIN, button->y + BUTTON_MARGIN, 
                          button->width - 2*BUTTON_MARGIN, button->height - 2*BUTTON_MARGIN, fill_color))
        return 1;

    // Draw icon
    uint16_t x, y;
    switch (button->icon.type) {
    case BUTTON_ICON_XPM:
        x = button->x + (button->width - button->icon.attributes.img.width) / 2;
        y = button->y + (button->height - button->icon.attributes.img.height) / 2;
        if (vb_draw_img(buf, button->icon.attributes.img, x, y) != OK)
            return 1;
        break;
    case BUTTON_ICON_CIRCLE:
        x = button->x + button->width/2;
        y = button->y + button->height/2;
        if (vb_draw_circle(buf, x, y, button->icon.attributes.circle.radius, button->icon.attributes.circle.color) != OK)
            return 1;
        break;
    default:
        break;
    } 

    return 0;
}

void button_unselect(button_t *button) {
    button->state = BUTTON_NORMAL;
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

/**@}*/
