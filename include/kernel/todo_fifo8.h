#pragma once
#include <define.h>
#include <libc.h>
#include <type.h>

struct FIFO8 {
  u8 *buf;
  int p, q, size, free, flags;
};

void fifo8_init(struct FIFO8 *fifo, int size, u8 *buf);
int  fifo8_put(struct FIFO8 *fifo, u8 data);
int  fifo8_get(struct FIFO8 *fifo);
int  fifo8_status(struct FIFO8 *fifo);