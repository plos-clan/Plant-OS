#pragma once
#include <libc.h>

#include "block.h"
#include "freelist.h"

struct mpool {
  void       *ptr;       // 指向内存区的指针
  size_t      size;      // 内存区总大小
  cb_reqmem_t cb_reqmem; //
  cb_delmem_t cb_delmem; //
  freelists_t freed;     //
};

dlimport size_t mpool_total_size(mpool_t pool);   // 占用的内存总大小
dlimport size_t mpool_alloced_size(mpool_t pool); // 分配的内存总大小

dlexport bool mpool_init(mpool_t pool, void *ptr, size_t size) {
  if (pool == null || ptr == null || size == 0) return false;
  if (size & (2 * sizeof(size_t) - 1)) return false;
  pool->ptr  = ptr;
  pool->size = size;
  return true;
}
dlexport void *mpool_alloc(mpool_t pool, size_t size) {
  size = PADDING(size);
  // void *freelists_match(pool->freed, size);
}
dlexport void   mpool_free(mpool_t pool, void *ptr) {}
dlexport size_t mpool_msize(mpool_t pool, void *ptr) {}
