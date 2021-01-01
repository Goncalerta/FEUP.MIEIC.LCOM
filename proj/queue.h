#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <lcom/lcf.h>

/** @file 
 * @brief File dedicated to queues.
 */

/** @defgroup queue queue
 * @{
 *
 * @brief Module dedicated to queues.
 */

/**
 * @brief Queue info.
 * 
 */
typedef struct queue_t {
    void *data; /*!< @brief Address of memory of the content of the queue. */
    size_t element_size; /*!< @brief Size of each queue element. */
    size_t front; /*!< @brief Index of queue front element. */
    size_t back; /*!< @brief Index of queue back element. */
    size_t size; /*!< @brief Queue size. */
    size_t capacity; /*!< @brief Queue capacity. */
} queue_t;

/**
 * @brief Initializes of a new queue.
 * 
 * @param queue address of memory of the queue to be initialized
 * @param element_size queue element size
 * @param capacity queue capacity
 * @return Return 0 upon success and non-zero otherwise
 */
int new_queue(queue_t *queue, size_t element_size, size_t capacity);

/**
 * @brief Frees the space allocated in memory to store the content of a given queue.
 * 
 * @param queue address of memory of the queue
 * @return Return 0 upon success and non-zero otherwise
 */
int delete_queue(queue_t *queue);

/**
 * @brief Checks if a given queue is full.
 * 
 * @param queue address of memory of the queue
 * @return Return true if the queue is full and false otherwise
 */
bool queue_is_full(queue_t *queue);

/**
 * @brief Checks if a given queue is empty.
 * 
 * @param queue address of memory of the queue
 * @return Return true if the queue is empty and false otherwise
 */
bool queue_is_empty(queue_t *queue);

/**
 * @brief Empties a given queue.
 * 
 * @param queue address of memory of the queue
 */
void queue_empty(queue_t *queue);

/**
 * @brief Gets the element at the front of the given queue.
 * 
 * @param queue address of memory of the queue
 * @param el address of memory to be initialized with element at the front of the queue
 * @return Return 0 upon success and non-zero otherwise
 */
int queue_top(queue_t *queue, void *el);

/**
 * @brief Adds an element to the back of the given queue.
 * 
 * If capacity is not enough, the queue is reallocated with double size.
 * 
 * @param queue address of memory of the queue
 * @param el address of memory of the element to be added
 * @return Return 0 upon success and non-zero otherwise
 */
int queue_push(queue_t *queue, void *el);

/**
 * @brief Removes the element at the front of the given queue.
 * 
 * @param queue address of memory of the queue
 * @return Return 0 upon success and non-zero otherwise
 */
int queue_pop(queue_t *queue);

/**@}*/

#endif /* _QUEUE_H */
