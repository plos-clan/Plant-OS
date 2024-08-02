#include <libc-base.h>

#include "area.h"
#include "block.h"
#include "freelist.h"

// mpool_free 中使用的临时函数
// 用于将内存块从空闲链表中分离
dlexport void _detach(mpool_t pool, freelist_t ptr) {
  int id = freelists_size2id(blk_size(ptr));
  if (id < 0) {
    pool->large_blk = freelist_detach(pool->large_blk, ptr);
  } else {
    freelists_detach(pool->freed, id, ptr);
  }
}

dlexport bool mpool_init(mpool_t pool, void *ptr, size_t size) {
  if (pool == null || ptr == null || size == 0) return false;
  if (size & (2 * sizeof(size_t) - 1)) return false;
  pool->ptr          = ptr;
  pool->size         = size;
  pool->alloced_size = 0;
  pool->cb_reqmem    = null;
  pool->cb_delmem    = null;
  pool->large_blk    = null;
  for (size_t i = 0; i < FREELISTS_NUM; i++) {
    pool->freed[i] = null;
  }
  allocarea_init(ptr, size, null);
  pool->large_blk = freelist_put(pool->large_blk, ptr + 2 * sizeof(size_t));
  return true;
}

dlexport void mpool_deinit(mpool_t pool) {
  if (pool == null) return;
  if (pool->cb_delmem) pool->cb_delmem(pool->ptr, pool->size);
  *pool = (struct mpool){};
}

dlexport size_t mpool_alloced_size(mpool_t pool) {
  return pool ? pool->alloced_size : 0;
}

dlexport size_t mpool_total_size(mpool_t pool) {
  return pool ? pool->size : 0;
}

dlexport void mpool_setcb(mpool_t pool, cb_reqmem_t reqmem, cb_delmem_t delmem) {
  if (pool == null) return;
  pool->cb_reqmem = reqmem;
  pool->cb_delmem = delmem;
}

static bool mpool_reqmem(mpool_t pool, size_t size) {
  if (pool->cb_reqmem == null) return false;
  size_t memsize = PADDING_16k(size);

  void *mem = pool->cb_reqmem(pool->ptr + pool->size, memsize);
  if (mem == null) return false;
  if (mem != pool->ptr) {
    if (pool->cb_delmem) pool->cb_delmem(mem, memsize);
    return false;
  }

  void *ptr   = allocarea_reinit(pool->ptr, pool->size, pool->size + memsize);
  pool->size += memsize;

  ptr = blk_trymerge(ptr, (blk_detach_t)_detach, pool);
  blk_setfreed(ptr, blk_size(ptr));
  pool->large_blk = freelist_put(pool->large_blk, ptr);
  return true;
}

dlexport void *mpool_alloc(mpool_t pool, size_t size) {
  if (size == 0) size = 2 * sizeof(size_t);
  size = PADDING(size);

  void *ptr = freelists_match(pool->freed, size);

  if (ptr == null) ptr = freelist_match(&pool->large_blk, size);

  if (ptr == null) { // 不足就分配
    if (!mpool_reqmem(pool, size)) return null;
    ptr = freelist_match(&pool->large_blk, size);
  }

  size_t realsize = blk_size(ptr);
  if (realsize >= size + 8 * sizeof(size_t)) {
    void *new_ptr = blk_split(ptr, size);
    blk_setfreed(new_ptr, blk_size(new_ptr));
    if (!freelists_put(pool->freed, new_ptr)) {
      pool->large_blk = freelist_put(pool->large_blk, new_ptr);
    }
  }

  blk_setalloced(ptr, blk_size(ptr));
  pool->alloced_size += blk_size(ptr);
  return ptr;
}

dlexport void mpool_free(mpool_t pool, void *ptr) {
  if (ptr == null) return;

  pool->alloced_size -= blk_size(ptr);

  ptr = blk_trymerge(ptr, (blk_detach_t)_detach, pool);
  blk_setfreed(ptr, blk_size(ptr));
  if (!freelists_put(pool->freed, ptr)) { //
    pool->large_blk = freelist_put(pool->large_blk, ptr);
  }
}

dlexport size_t mpool_msize(mpool_t pool, void *ptr) {
  if (ptr == null) return 0;
  return blk_size(ptr);
}
