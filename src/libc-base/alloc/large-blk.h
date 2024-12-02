#pragma once
#include <libc-base.h>

#include "freelist.h"

struct large_blk {
  large_blk_t next;
  large_blk_t prev;
  void       *ptr;
  size_t      size;
};

finline large_blk_t *large_blk_blokp(large_blks_t blks, void *ptr) __attr(const);

/**
 *\brief 获取大内存块应该放入的空闲链表
 *
 *\param blks     大内存块空闲链表 (组)
 *\param ptr      大内存块指针
 *\return 大块内存空闲链表指针
 */
finline large_blk_t *large_blk_blokp(large_blks_t blks, void *ptr) {
  size_t hash = ((size_t)ptr / SIZE_4k | (size_t)ptr / SIZE_4k / LARGEBLKLIST_NUM / 2);
  return blks + hash % LARGEBLKLIST_NUM;
}

finline large_blk_t large_blk_put(large_blks_t blks, void *ptr, size_t size) {
  large_blk_t *blk_p = large_blk_blokp(blks, ptr);
  large_blk_t  blk   = malloc(sizeof(struct large_blk));
  if (blk == null) return null;
  blk->ptr  = ptr;
  blk->size = size;
  freelist_put((freelist_t *)blk_p, (freelist_t)blk);
  return blk;
}

finline large_blk_t large_blk_find(large_blks_t blks, void *ptr) {
  large_blk_t *blk_p = large_blk_blokp(blks, ptr);
  for (large_blk_t blk = *blk_p; blk; blk = blk->next) {
    if (blk->ptr == ptr) return blk;
  }
  return null;
}

/**
 *\brief 分配大块内存
 *
 *\param size     请求的内存大小 (保证其大于 ALLOC_LARGE_BLK_SIZE)
 *\param blks     大内存块空闲链表 (组)
 *\param reqmem   请求内存的回调函数
 *\param delmem   释放内存的回调函数
 *\return 分配的内存地址
 */
finline void *large_blk_alloc(size_t size, large_blks_t blks, cb_reqmem_t reqmem,
                              cb_delmem_t delmem) {
  size      = PADDING_4k(size);
  void *ptr = reqmem(null, size);
  if (ptr == null) return null;
  large_blk_t blk = large_blk_put(blks, ptr, size);
  if (blk == null) {
    delmem(ptr, size);
    return null;
  }
  blk->ptr  = ptr;
  blk->size = size;
  blk->prev = null;
  blk->next = null;
  return ptr;
}

/**
 *\brief 释放大块内存
 *       可以传入任何指针，非大内存块会返回 false
 *
 *\param blks     大内存块空闲链表 (组)
 *\param ptr      大内存块指针
 *\param delmem   释放内存的回调函数
 *\return 是否释放成功
 */
finline bool large_blk_free(large_blks_t blks, void *ptr, cb_delmem_t delmem) {
  large_blk_t *blk_p = large_blk_blokp(blks, ptr);
  auto         blk   = large_blk_find(blks, ptr);
  if (blk == null) return false;
  *blk_p = (large_blk_t)freelist_detach((freelist_t)*blk_p, (freelist_t)blk);
  delmem(blk->ptr, blk->size);
  free(blk);
  return true;
}
