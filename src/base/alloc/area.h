#pragma once
#include <base.h>

#include "block.h"

finline void allocarea_init(void *_ptr, size_t size, void *addr) {
  size_t *ptr  = _ptr;
  size_t  len  = size / sizeof(size_t);
  size_t  data = (size_t)addr | AREA_FLAG | (size == 2 * 1024 * 1024 ? SIZE_FLAG : 0);
  ptr[0] = ptr[len - 1] = data; // 将首尾设为占位符
  blk_setsize(ptr + 2, size - 4 * sizeof(size_t));
  blk_setfreed(ptr + 2, size - 4 * sizeof(size_t));
}

finline void allocarea_reinit(void *_ptr, size_t oldsize, size_t newsize) {
  if (newsize == oldsize) return;
  if (newsize > oldsize) { // 增大
  } else {                 // 减小
  }

  size_t *ptr  = _ptr;
  size_t  len  = newsize / sizeof(size_t);
  size_t  data = (ptr[0] & ~FLAG_BITS) | AREA_FLAG | (newsize == 2 * 1024 * 1024 ? SIZE_FLAG : 0);
  ptr[0] = ptr[len - 1] = data; // 将首尾设为占位符
}
