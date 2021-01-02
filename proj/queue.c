#include <lcom/lcf.h>

#include "queue.h"

// Queue class implementation
struct queue {
    void *data; // Address of memory of the content of the queue.
    size_t element_size; // Size of each queue element.
    size_t front; // Index of queue front element.
    size_t back; // Index of queue back element.
    size_t size; // Queue size.
    size_t capacity; // Queue capacity.
};

queue_t *new_queue(size_t element_size, size_t capacity) {
    if (element_size == 0)
        return NULL;

    queue_t *queue = malloc(sizeof(queue_t));
    if (queue == NULL)
        return NULL;
    

    queue->element_size = element_size;
    queue->capacity = capacity;
    queue->data = malloc(element_size * capacity);
    if (queue->data == NULL) {
        return NULL;
    }
    queue->front = queue->back = queue->size = 0;

    return queue;
}

void delete_queue(queue_t *queue) {
    if (queue == NULL)
        return;

    if (queue->data == NULL)
        return;
        
    free(queue->data);
    free(queue);
}

// Returns a pointer to the element of the given queue at the given index.
static void *queue_index(queue_t *queue, size_t i) {
    return (uint8_t*) queue->data + i * queue->element_size;
}

// Doubles the capacity of the queue without losing its current data.
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
