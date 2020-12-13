#ifndef __CLUE_H
#define __CLUE_H

#include <lcom/lcf.h>
#include "graphics.h"

typedef struct word_clue_t {
    char *word;
    char *clue;
    size_t size;
    size_t missing;
    uint16_t width, height;
} word_clue_t;

int new_word_clue(word_clue_t *clue, char *word);

int word_clue_draw(word_clue_t *clue, frame_buffer_t buf, uint16_t x, uint16_t y);

int word_clue_hint(word_clue_t *clue);

void free_word_clue(word_clue_t *clue);

#endif /* __CLUE_H */
