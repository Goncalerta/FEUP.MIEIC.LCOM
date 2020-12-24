#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <lcom/lcf.h>

typedef struct queue_t {
    void *data;
    size_t element_size;
    size_t front, back;
    size_t size, capacity;
} queue_t;

int new_queue(queue_t *queue, size_t element_size, size_t capacity);
int delete_queue(queue_t *queue);

bool queue_is_full(queue_t *queue);
bool queue_is_empty(queue_t *queue);
void queue_empty(queue_t *queue);

void queue_push(queue_t *queue, void *el);
void queue_pop(queue_t *queue, void *el);

#endif /* _QUEUE_H */
