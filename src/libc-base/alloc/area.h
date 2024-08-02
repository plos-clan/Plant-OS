#pragma once
#include <libc-base.h>

#include "block.h"

// 获取分配区大小，仅对多分配区有效
#define allocarea_size(ptr) (((size_t *)ptr)[0] & SIZE_FLAG ? SIZE_2M : SIZE_4k)

finline void allocarea_init(void *_ptr, size_t size, void *addr) {
  size_t *ptr  = _ptr;
  size_t  len  = size / sizeof(size_t);
  size_t  data = (size_t)addr | AREA_FLAG | (size == SIZE_2M ? SIZE_FLAG : 0);
  ptr[0] = ptr[len - 1] = data; // 将首尾设为占位符
  blk_setsize(ptr + 2, size - 4 * sizeof(size_t));
  blk_setfreed(ptr + 2, size - 4 * sizeof(size_t));
}

finline void *allocarea_reinit(void *_ptr, size_t oldsize, size_t newsize) {
  if (newsize == oldsize) return null;

  size_t *ptr  = _ptr;
  size_t  len  = newsize / sizeof(size_t);
  size_t  data = (ptr[0] & ~FLAG_BITS) | AREA_FLAG | (newsize == SIZE_2M ? SIZE_FLAG : 0);
  ptr[0] = ptr[len - 1] = data; // 将首尾设为占位符

  if (newsize < oldsize) return null;

  void *blk = _ptr + oldsize + sizeof(size_t);
  blk_setsize(blk, newsize - oldsize - 2 * sizeof(size_t));
  return blk;
}
