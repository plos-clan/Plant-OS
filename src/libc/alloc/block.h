#pragma once
#include <define.h>
#include <type.h>

#define FREE_FLAG ((size_t)1)
#define AREA_FLAG ((size_t)2)
#define SIZE_FLAG ((size_t)4)
#define FLAG_BITS ((size_t)7) // 所有标志位

#define PADDING(size) (((size) + sizeof(size_t) - 1) & ~sizeof(size_t))

#define blk_prevtail(ptr)       (((size_t *)ptr)[-2])
#define blk_head(ptr)           (((size_t *)ptr)[-1])
#define blk_tail(ptr, size)     (((size_t *)(ptr + size))[0])
#define blk_nexthead(ptr, size) (((size_t *)(ptr + size))[1])

#define blk_noprev(ptr)       ((bool)(blk_prevtail(ptr) & AREA_FLAG))
#define blk_nonext(ptr, size) ((bool)(blk_nexthead(ptr, size) & AREA_FLAG))

#define blk_freed(ptr)   (blk_head(ptr) & FREE_FLAG)
#define blk_alloced(ptr) (!blk_freed(ptr))

finline void blk_setalloced(void *ptr, size_t size) {
  blk_head(ptr)       &= ~FREE_FLAG;
  blk_tail(ptr, size) &= ~FREE_FLAG;
}
finline void blk_setfreed(void *ptr, size_t size) {
  blk_head(ptr)       |= FREE_FLAG;
  blk_tail(ptr, size) |= FREE_FLAG;
}

#define blk_size(ptr) (blk_head(ptr) & ~FLAG_BITS)

finline void blk_setsize(void *ptr, size_t size) {
  ((size_t *)ptr)[-1]     = size;
  *(size_t *)(ptr + size) = size;
}

// 判断分配区大小是否为 2M，仅对多分配区有效
#define blk_area_is_2m(ptr) (blk_head(ptr) & SIZE_FLAG)
#define blk_area_is_4k(ptr) (!blk_area_is_2m(ptr))

finline void blk_set_area_4k(void *ptr, size_t size) {
  blk_head(ptr)       &= ~SIZE_FLAG;
  blk_tail(ptr, size) &= ~SIZE_FLAG;
}
finline void blk_set_area_2m(void *ptr, size_t size) {
  blk_head(ptr)       |= SIZE_FLAG;
  blk_tail(ptr, size) |= SIZE_FLAG;
}

// 获取分配区头指针，仅对多分配区有效
finline void *blk_areaptr(void *ptr) {
  if (blk_area_is_2m(ptr)) {
    return (void *)((size_t)ptr & ~(size_t)(2 * 1024 * 1024 - 1));
  } else {
    return (void *)((size_t)ptr & ~(size_t)(4096 - 1));
  }
}

// 获取上一个块的指针
finline void *blk_prev(void *ptr) {
  if (blk_noprev(ptr)) return null;
  size_t prevsize = blk_prevtail(ptr) & ~FLAG_BITS;
  return ptr - 2 * sizeof(size_t) - prevsize;
}
// 获取下一个块的指针
finline void *blk_next(void *ptr) {
  size_t size = blk_size(ptr);
  if (blk_nonext(ptr, size)) return null;
  return ptr + size + 2 * sizeof(size_t);
}

finline void *blk_mergeprev(void *ptr) {
  void  *prev = blk_prev(ptr);
  size_t size = blk_size(ptr) + blk_size(prev) + 2 * sizeof(size_t);
  // TODO 从空闲链表删除
  blk_setsize(prev, size);
  return prev;
}
finline void *blk_mergenext(void *ptr) {
  void  *next = blk_next(ptr);
  size_t size = blk_size(ptr) + blk_size(next) + 2 * sizeof(size_t);
  // TODO 从空闲链表删除
  blk_setsize(ptr, size);
  return ptr;
}
finline void *blk_trymerge(void *ptr) {
  size_t size = blk_size(ptr);
  if (!blk_nonext(ptr, size) && blk_nexthead(ptr, size) & FREE_FLAG) ptr = blk_mergenext(ptr);
  if (!blk_noprev(ptr) && blk_prevtail(ptr) & FREE_FLAG) ptr = blk_mergeprev(ptr);
  return ptr;
}
