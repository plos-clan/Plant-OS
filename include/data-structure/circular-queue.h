#pragma once
#include "base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define CIRCQUEUE_IMPLEMENTATION
#endif

typedef struct cir_queue {
  u8    *buf;
  size_t tail;
  size_t head;
  size_t free;
  size_t size;
  size_t flags;
} *cir_queue_t;

#define FLAGS_OVERRUN 0x0001

#ifdef CIRCQUEUE_IMPLEMENTATION
#  define extern static
#endif

extern void cir_queue_init(cir_queue_t queue, size_t size, u8 *buf);
extern int  cir_queue_put(cir_queue_t queue, u8 data);
extern int  cir_queue_get(cir_queue_t queue);
extern int  cir_queue_len(cir_queue_t queue);

#ifdef CIRCQUEUE_IMPLEMENTATION
#  undef extern
#endif

#ifdef CIRCQUEUE_IMPLEMENTATION

static void cir_queue_init(cir_queue_t queue, size_t size, u8 *buf) {
  queue->size  = size;
  queue->buf   = buf;
  queue->free  = size;
  queue->flags = 0;
  queue->tail  = 0;
  queue->head  = 0;
}

static int cir_queue_put(cir_queue_t queue, u8 data) {
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

static int cir_queue_get(cir_queue_t queue) {
  int data;
  if (queue->free == queue->size) return -1; // 如果缓冲区为空则返回
  data = queue->buf[queue->head];
  queue->head++;
  if (queue->head == queue->size) { queue->head = 0; }
  queue->free++;
  return data;
}

static int cir_queue_len(cir_queue_t queue) {
  if (queue == null) return 0;
  return queue->size - queue->free;
}

static void cir_queue_copyto(cir_queue_t src, cir_queue_t dst) {
  if (dst->size != src->size) return;
  dst->free  = src->free;
  dst->flags = src->flags;
  dst->tail  = src->tail;
  dst->head  = src->head;
  memcpy(dst->buf, src->buf, src->size);
}

#  undef CIRCQUEUE_IMPLEMENTATION
#endif
