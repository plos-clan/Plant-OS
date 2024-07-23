#pragma once
#include "../string/mem.h"

#ifdef ALL_IMPLEMENTATION
#  define CIRCULAR_QUEUE_IMPLEMENTATION
#endif

typedef struct circular_queue {
  u8 *buf;
  int p, q, size, free, flags;
} *circular_queue_t;

#ifdef CIRCULAR_QUEUE_IMPLEMENTATION
#  define extern static
#endif

extern void circular_queue_init(circular_queue_t queue, int size, unsigned char *buf);
extern int  circular_queue_put(circular_queue_t queue, unsigned char data);
extern int  circular_queue_get(circular_queue_t queue);
extern int  circular_queue_len(circular_queue_t queue);

#ifdef CIRCULAR_QUEUE_IMPLEMENTATION
#  undef extern
#endif

#ifdef CIRCULAR_QUEUE_IMPLEMENTATION

#  define FLAGS_OVERRUN 0x0001

static void circular_queue_init(circular_queue_t queue, int size, unsigned char *buf) {
  queue->size  = size;
  queue->buf   = buf;
  queue->free  = size; /* 缓冲区大小 */
  queue->flags = 0;
  queue->p     = 0; /* 下一个数据写入位置 */
  queue->q     = 0; /* 下一个数据读出位置 */
}

static int circular_queue_put(circular_queue_t queue, unsigned char data) {
  if (queue->free == 0) {
    queue->flags |= FLAGS_OVERRUN;
    return -1;
  }
  queue->buf[queue->p] = data;
  queue->p++;
  if (queue->p == queue->size) { queue->p = 0; }
  queue->free--;
  return 0;
}

static int circular_queue_get(circular_queue_t queue) {
  int data;
  if (queue->free == queue->size) return -1; // 如果缓冲区为空则返回
  data = queue->buf[queue->q];
  queue->q++;
  if (queue->q == queue->size) { queue->q = 0; }
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
  dst->p     = src->p;
  dst->q     = src->q;
  memcpy(dst->buf, src->buf, src->size);
}

#  undef CIRCULAR_QUEUE_IMPLEMENTATION
#endif
