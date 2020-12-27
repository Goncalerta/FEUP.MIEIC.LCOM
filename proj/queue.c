#include <lcom/lcf.h>

#include "queue.h"

// For debug purposes
// static void print_queue(queue_t *queue) {
//     printf("QUEUE x%d %d/%d: ", queue->element_size, queue->size, queue->capacity);
    
//     for (size_t i = 0; i < queue->size; i++) {
//         printf("0x");
//         for (size_t j = 0; j < queue->element_size; j++) {
//             printf("%02x",((uint8_t *) queue->data)[(queue->front + i)*queue->element_size + j]);
//         }
        
//         printf(" ");
//     }
    
//     printf("\n");
// }

int new_queue(queue_t *queue, size_t element_size, size_t capacity) {
    if (element_size == 0)
        return 1;

    queue->element_size = element_size;
    queue->capacity = capacity;
    queue->data = malloc(element_size * capacity);
    if (queue->data == NULL) {
        return 1;
    }
    queue->front = queue->back = queue->size = 0;

    return 0;
}

int delete_queue(queue_t *queue) {
    if (queue->data == NULL)
        return 1;
    free(queue->data);
    queue->data = NULL;

    return 0;
}

static void *queue_index(queue_t *queue, size_t i) {
    return (uint8_t*) queue->data + i * queue->element_size;
}

static int queue_resize(queue_t *queue) {
    size_t new_capacity = 2 * queue->capacity;
    
    int *data = realloc(queue->data, new_capacity * queue->element_size);
    if (data == NULL)
        return 1;
    queue->data = data;
    
    if (queue->front > queue->back || queue_is_full(queue)) {
        memcpy(queue_index(queue, queue->capacity),
               queue_index(queue, 0),
               queue->back * queue->element_size);
        queue->back += queue->size;
    }
    
    queue->capacity = new_capacity;
    return 0;
}

bool queue_is_full(queue_t *queue) {
    return queue->size == queue->capacity;
}

bool queue_is_empty(queue_t *queue) {
    return queue->size == 0;
}

void queue_empty(queue_t *queue) {
    queue->front = queue->back = queue->size = 0;
}

int queue_top(queue_t *queue, void *el) {
    if (queue_is_empty(queue))
        return 1;

    memcpy(el, queue_index(queue, queue->front), queue->element_size);
    return 0;
}

int queue_push(queue_t *queue, void *el) {
    if (queue_is_full(queue)) {
        if (queue_resize(queue) != OK)
            return 1;
    }

    memcpy(queue_index(queue, queue->back), el, queue->element_size);
    queue->back = (queue->back + 1) % queue->capacity;
    queue->size++;
    return 0;
}

int queue_pop(queue_t *queue) {
    if (queue_is_empty(queue))
        return 1;

    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return 0;
}
