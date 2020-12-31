#ifndef __CLUE_H
#define __CLUE_H

#include <lcom/lcf.h>
#include "graphics.h"

/** @defgroup clue clue
 * @{
 *
 * Module dedicated to word clues.
 */

/**
 * @brief Word clue
 * 
 */
typedef struct word_clue_t {
    char *word; /*!< Address of memory of the word to give clues of. */
    char *clue; /*!< Address of memory of the clues already given. */
    size_t size; /*!< Size of word. */
    size_t missing; /*!< Number of chars that were not hinted. */
    uint16_t width; /*!< Width of the clue on the screen. */
    uint16_t height; /*!< Height of the clue on the screen. */
} word_clue_t;

/**
 * @brief Initiates the contents of a new word clue.
 * 
 * @param clue address of memory to the word clue
 * @param word address of memory of the word to be used
 * @return Return 0 upon success and non-zero otherwise
 */
int new_word_clue(word_clue_t *clue, const char *word);

/**
 * @brief Draws a given word clue to the given buffer on the given coordinates.
 * 
 * @param clue address of memory to the word clue
 * @param buf buffer
 * @param x x coordinate
 * @param y y coordinate
 * @return Return 0 upon success and non-zero otherwise
 */
int word_clue_draw(word_clue_t *clue, frame_buffer_t buf, uint16_t x, uint16_t y);

/**
 * @brief Hints a char in the given word clue.
 * 
 * @param clue address of memory to the word clue
 * @param pos address of memory to be initialized with the word missing position that was clued
 * @return Return 0 upon success and non-zero otherwise
 */
int word_clue_hint(word_clue_t *clue, size_t *pos);

/**
 * @brief Hints a char in the given word clue in the given position.
 * 
 * @param clue address of memory to the word clue
 * @param pos position to be clued
 * @return Return 0 upon success and non-zero otherwise
 */
int word_clue_hint_at(word_clue_t *clue, size_t pos);

/**
 * @brief Makes the whole word revealed in the given word clue.
 * 
 * @param clue address of memory to the word clue
 */
void clue_reveal(word_clue_t *clue);

/**
 * @brief Frees the space allocated in memory to store the content of a given word clue.
 * 
 * @param clue address of memory of the word clue
 */
void delete_word_clue(word_clue_t *clue);

/**@}*/

#endif /* __CLUE_H */
