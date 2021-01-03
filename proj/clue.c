#include <lcom/lcf.h>
#include "clue.h"
#include "font.h"

#define CLUE_CHAR_SPACING 10
#define CLUE_BAR_MARGIN 5
#define RECTANGLE_HEIGHT 5

/** @defgroup clue clue
 * @{
 *
 */

/**
 * @brief Word clue class implementation.
 * 
 */
struct word_clue {
    char *word; /**< @brief Address of memory of the word to give clues of. */
    char *clue; /**< @brief Address of memory of the clues already given. */
    size_t size; /**< @brief Size of word. */
    size_t missing; /**< @brief Number of chars that were not hinted. */
};

word_clue_t *new_word_clue(const char *word) {
    word_clue_t *clue = malloc(sizeof(word_clue_t));
    if (clue == NULL)
        return NULL;
    clue->word = malloc((strlen(word) + 1) * sizeof(char));
    if (clue->word == NULL) {
        free(clue);
        return NULL;
    }
    strcpy(clue->word, word);
    clue->size = strlen(word);
    clue->missing = clue->size;

    clue->clue = malloc(sizeof(char) * clue->size + 1);
    if (clue->clue == NULL) {
        free(clue->word);
        free(clue);
        return NULL;
    }

    memset(clue->clue, '?', clue->size);
    clue->clue[clue->size] = '\0';
    
    return clue;
}

uint16_t word_clue_get_width(word_clue_t *clue) {
    return clue->size * (FONT_CHAR_WIDTH + CLUE_CHAR_SPACING) - CLUE_CHAR_SPACING;
}

int word_clue_draw(word_clue_t *clue, frame_buffer_t buf, uint16_t x, uint16_t y) {
    for (size_t i = 0; i < clue->size; i++) {
        uint16_t current_x = x + i * (FONT_CHAR_WIDTH + CLUE_CHAR_SPACING);
        if (vb_draw_rectangle(buf, current_x-2, y-2, FONT_CHAR_WIDTH+4, FONT_CHAR_HEIGHT+ CLUE_BAR_MARGIN+RECTANGLE_HEIGHT +4, 0xffffff) != OK)
            return 1;

        char letter = clue->clue[i];

        if (letter != ' ') {
            if (vb_draw_rectangle(buf, current_x, y + FONT_CHAR_HEIGHT + CLUE_BAR_MARGIN, FONT_CHAR_WIDTH, RECTANGLE_HEIGHT, 0x000000) != OK)
                return 1;
        }
        
        if (letter != '?') {
            if (font_draw_char(buf, letter, current_x, y))
                return 1;
        }
    }

    return 0;
}

int word_clue_hint(word_clue_t *clue, size_t *pos) {
    // Hints should not reveal the whole word. If there is only one
    // character left, it should not be revealed.
    if (clue->missing <= 1)
        return 1;
    size_t hint_pos = rand() % clue->missing;
    
    for (size_t i = 0; i < clue->size; i++) {
        if (clue->clue[i] != '?') 
            continue;

        if (hint_pos == 0) {
            clue->clue[i] = clue->word[i];
            clue->missing--;
            *pos = i;
            break;
        }
        hint_pos--;
    }

    return 0;
}

int word_clue_hint_at(word_clue_t *clue, size_t pos) {
    if (pos >= clue->size)
        return 1;
    
    if (clue->clue[pos] == '?') {
        clue->missing--;
    }
    clue->clue[pos] = clue->word[pos];

    return 0;
}

void clue_reveal(word_clue_t *clue) {
    strcpy(clue->clue, clue->word);
    clue->missing = 0;
}

void delete_word_clue(word_clue_t *clue) {
    if (clue == NULL)
        return;

    if (clue->word != NULL)
        free(clue->word);
        
    if (clue->clue != NULL) 
        free(clue->clue);

    free(clue);
}

/**@}*/
