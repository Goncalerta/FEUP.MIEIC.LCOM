#ifndef __CANVAS_H
#define __CANVAS_H

#include <lcom/lcf.h>

#include "keyboard.h"

/** @defgroup canvas canvas
 * @{
 *
 * @brief Module dedicated to the canvas used to draw.
 */

/**
 * @brief Enumerated type for specifying the state of the canvas.
 * 
 */
typedef enum canvas_state_t {
    CANVAS_STATE_NORMAL, /*!< Canvas is in normal/base state. */
    CANVAS_STATE_HOVERING, /*!< Canvas is not being pressed but the cursor is hovering it. */
    CANVAS_STATE_PRESSING_LB, /*!< Canvas is being pressed by the left button of the mouse. */
    CANVAS_STATE_PRESSING_RB, /*!< Canvas is being pressed by the right button of the mouse. */
} canvas_state_t;

/**
 * @brief Atom of a canvas stroke.
 * 
 */
typedef struct stroke_atom {
    uint16_t x; /*!< x coordinate of the atom. */
    uint16_t y; /*!< y coordinate of the atom. */
} stroke_atom;

/**
 * @brief Canvas stroke.
 * 
 */
typedef struct stroke {
    uint32_t color; /*!< Color of the stroke. */
    uint16_t thickness; /*!< Thickness of the stroke. */
    size_t num_atoms; /*!< Number of atoms of the stroke. */
    stroke_atom *atoms; /*!< Address of memory of the strokes. */
    struct stroke *next; /*!< Address of memory of the stroke drawn after this. */ // TODO is this comment correct?
    struct stroke *prev; /*!< Address of memory of the stroke drawn before this. */ // TODO is this comment correct?
} stroke;


/**
 * @brief Checks if the canvas is initialized.
 * 
 * @return Return true if the canvas is initialized and false otherwise
 */
bool canvas_is_initialized();

/**
 * @brief Initiates the canvas by setting up its initial content.
 * 
 * @param width canvas width
 * @param height canvas height
 * @param enabled is the canvas enabled
 * @return Return 0 upon success and non-zero otherwise
 */
int canvas_init(uint16_t width, uint16_t height, bool enabled);

/**
 * @brief Exits the canvas and frees the space allocated in memory to store its buffer.
 * 
 */
void canvas_exit();

/**
 * @brief Draws the canvas to the back buffer.
 * 
 * @param y y coordinate of the back buffer to start drawing
 * @return Return 0 upon success and non-zero otherwise
 */
int canvas_draw_frame(uint16_t y);

/**
 * @brief Clears all the canvas strokes.
 * 
 */
void clear_canvas();

/**
 * @brief Checks if canvas is enabled.
 * 
 * @return Return true if canvas is enabled and false otherwise
 */
bool canvas_is_enabled();

/**
 * @brief Starts a new stroke in the canvas.
 * 
 * @param color stroke color
 * @param thickness stroke thickness
 * @return Return 0 upon success and non-zero otherwise
 */
int canvas_new_stroke(uint32_t color, uint16_t thickness);

/**
 * @brief Adds a new atom to the current stroke.
 * 
 * @param x x coordinate of the atom
 * @param y y coordinate of the atom
 * @return Return 0 upon success and non-zero otherwise
 */
int canvas_new_stroke_atom(uint16_t x, uint16_t y);

/**
 * @brief Undos a stroke. // TODO better description?
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int canvas_undo_stroke();

/**
 * @brief Redos a stroke.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int canvas_redo_stroke();

/**
 * @brief Checks if the given coordinates are inside the limits of the canvas.
 * 
 * @param x x coordinate
 * @param y y coordinate
 * @return Return true if the coordinates are inside the canvas and false otherwise
 */
bool canvas_is_hovering(uint16_t x, uint16_t y);

/**
 * @brief Updates the state of the canvas according to given mouse info.
 * 
 * @param hovering canvas is being hovered
 * @param lb is left button pressed
 * @param rb is right button pressed
 * @return Return 0 upon success and non-zero otherwise
 */
int canvas_update_state(bool hovering, bool lb, bool rb);

/**
 * @brief Updates the canvas according to a keyboard event.
 * 
 * @param kbd_event keyboard event
 * @return Return 0 upon success and non-zero otherwise
 */
int canvas_react_kbd(kbd_event_t kbd_event);

/**
 * @brief Gets the current canvas state.
 * 
 * @return Return the current canvas state
 */
canvas_state_t canvas_get_state();

/**@}*/

#endif /* __CANVAS_H */
