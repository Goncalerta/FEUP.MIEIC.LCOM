#ifndef __CLUE_H
#define __CLUE_H

#include <lcom/lcf.h>
#include "graphics.h"

/** @file 
 * @brief File dedicated to word clues.
 */

/** @defgroup clue clue
 * @{
 *
 * @brief Module dedicated to word clues.
 */

struct word_clue;

/**
 * @brief Word clue class.
 * 
 */
typedef struct word_clue word_clue_t;

/**
 * @brief Initializes new word clue.
 * 
 * @param word address of memory of the word to be used
 * @return Address of memory of the word clue initialized, or NULL if an error occurred.
 */
word_clue_t *new_word_clue(const char *word);

/**
 * @brief Gets the width in pixels of a word clue.
 * 
 * @param clue address of memory to the word clue
 * @return Return the width of the word clue
 */
uint16_t word_clue_get_width(word_clue_t *clue);

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
 * @brief Randomly hints a new char in the given word clue.
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
