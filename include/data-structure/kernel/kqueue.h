#pragma once
#include <define.h>

#ifdef __cplusplus
#  error "内核不允许使用 C++"
#endif

// 未完成

typedef struct kqueue {
  void **buffer;
  size_t head;
  size_t tail;
  size_t capacity;
  size_t size;
} *kqueue_t;

finline void kqueue_init(kqueue_t queue, void **buffer, size_t capacity) {
  queue->buffer   = buffer;
  queue->head     = 0;
  queue->tail     = 0;
  queue->capacity = capacity;
  atom_store(&queue->size, 0);
}

finline bool kqueue_enqueue(kqueue_t queue, void *item) {
  size_t size = atom_load(&queue->size);
  if (size == queue->capacity) {
    return false; // Queue is full
  }
  queue->buffer[queue->tail] = item;
  queue->tail                = (queue->tail + 1) % queue->capacity;
  atom_add(&queue->size, 1);
  return true;
}

finline bool kqueue_dequeue(kqueue_t queue, void **item) {
  size_t size = atomic_load(&queue->size);
  if (size == 0) {
    return false; // Queue is empty
  }
  *item       = queue->buffer[queue->head];
  queue->head = (queue->head + 1) % queue->capacity;
  atom_sub(&queue->size, 1);
  return true;
}

finline size_t kqueue_size(kqueue_t queue) {
  return atom_load(&queue->size);
}
