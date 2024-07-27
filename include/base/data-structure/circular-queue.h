#pragma once
#include "../string/mem.h"

#ifdef ALL_IMPLEMENTATION
#  define CIRCQUEUE_IMPLEMENTATION
#endif

typedef struct circular_queue {
  u8    *buf;
  size_t tail;
  size_t head;
  size_t free;
  size_t size;
  size_t flags;
} *circular_queue_t;

#define FLAGS_OVERRUN 0x0001

#ifdef CIRCQUEUE_IMPLEMENTATION
#  define extern static
#endif

extern void circular_queue_init(circular_queue_t queue, size_t size, u8 *buf);
extern int  circular_queue_put(circular_queue_t queue, u8 data);
extern int  circular_queue_get(circular_queue_t queue);
extern int  circular_queue_len(circular_queue_t queue);

#ifdef CIRCQUEUE_IMPLEMENTATION
#  undef extern
#endif

#ifdef CIRCQUEUE_IMPLEMENTATION

static void circular_queue_init(circular_queue_t queue, size_t size, u8 *buf) {
  queue->size  = size;
  queue->buf   = buf;
  queue->free  = size;
  queue->flags = 0;
  queue->tail  = 0;
  queue->head  = 0;
}

static int circular_queue_put(circular_queue_t queue, u8 data) {
  if (queue->free == 0) {
    queue->flags |= FLAGS_OVERRUN;
    return -1;
  }
  queue->buf[queue->tail] = data;
  queue->tail++;
  if (queue->tail == queue->size) { queue->tail = 0; }
  queue->free--;
  return 0;
}

static int circular_queue_get(circular_queue_t queue) {
  int data;
  if (queue->free == queue->size) return -1; // 如果缓冲区为空则返回
  data = queue->buf[queue->head];
  queue->head++;
  if (queue->head == queue->size) { queue->head = 0; }
  queue->free++;
  return data;
}

static int circular_queue_len(circular_queue_t queue) {
  if (queue == null) return 0;
  return queue->size - queue->free;
}

static void circular_queue_copyto(circular_queue_t src, circular_queue_t dst) {
  if (dst->size != src->size) return;
  dst->free  = src->free;
  dst->flags = src->flags;
  dst->tail  = src->tail;
  dst->head  = src->head;
  memcpy(dst->buf, src->buf, src->size);
}

#  undef CIRCQUEUE_IMPLEMENTATION
#endif
