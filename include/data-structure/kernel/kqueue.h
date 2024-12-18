#pragma once
#include <define.h>

// 未完成

typedef struct {
  void **buffer;
  size_t head;
  size_t tail;
  size_t capacity;
  size_t size;
} kqueue_t;

static inline void kqueue_init(kqueue_t *queue, void **buffer, size_t capacity) {
  queue->buffer   = buffer;
  queue->head     = 0;
  queue->tail     = 0;
  queue->capacity = capacity;
  atomic_store(&queue->size, 0);
}

static inline bool kqueue_enqueue(kqueue_t *queue, void *item) {
  size_t size = atomic_load(&queue->size);
  if (size == queue->capacity) {
    return false; // Queue is full
  }
  queue->buffer[queue->tail] = item;
  queue->tail                = (queue->tail + 1) % queue->capacity;
  atomic_fetch_add(&queue->size, 1);
  return true;
}

static inline bool kqueue_dequeue(kqueue_t *queue, void **item) {
  size_t size = atomic_load(&queue->size);
  if (size == 0) {
    return false; // Queue is empty
  }
  *item       = queue->buffer[queue->head];
  queue->head = (queue->head + 1) % queue->capacity;
  atomic_fetch_sub(&queue->size, 1);
  return true;
}

static inline size_t kqueue_size(kqueue_t *queue) {
  return atomic_load(&queue->size);
}
