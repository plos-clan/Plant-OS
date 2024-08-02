#include <libc-base.h>

#include "area.h"
#include "block.h"
#include "freelist.h"

// mman_free 中使用的临时函数
// 用于将内存块从空闲链表中分离
dlexport void _detach(mman_t man, freelist_t ptr) {
  int id = freelists_size2id(blk_size(ptr));
  if (id < 0) {
    man->large_blk = freelist_detach(man->large_blk, ptr);
  } else {
    freelists_detach(man->freed, id, ptr);
  }
}

dlexport bool mman_init(mman_t man, void *ptr, size_t size) {
  if (man == null || ptr == null || size == 0) return false;
  if (size != SIZE_2M && size != SIZE_4k) return false;
  man->main.ptr          = ptr;
  man->main.alloced_size = null;
  man->main.next         = null;
  man->size              = size;
  man->alloced_size      = 0;
  man->cb_reqmem         = null;
  man->cb_delmem         = null;
  man->large_blk         = null;
  for (size_t i = 0; i < FREELISTS_NUM; i++) {
    man->freed[i] = null;
  }
  allocarea_init(ptr, size, null);
  man->large_blk = freelist_put(man->large_blk, ptr + 2 * sizeof(size_t));
  return true;
}

dlexport void mman_deinit(mman_t man) {
  if (man == null) return;
  if (man->cb_delmem) {
    for (mman_pool_t p = man->main.next, n = p->next; p; p = n, n = p->next) {
      man->cb_delmem(p->ptr, allocarea_size(p->ptr));
    }
    man->cb_delmem(man->main.ptr, allocarea_size(man->main.ptr));
  }
  *man = (struct mman){};
}

dlexport size_t mman_alloced_size(mman_t man) {
  return man ? man->alloced_size : 0;
}

dlexport size_t mman_total_size(mman_t man) {
  return man ? man->size : 0;
}

dlexport void mman_setcb(mman_t man, cb_reqmem_t reqmem, cb_delmem_t delmem) {
  if (man == null) return;
  man->cb_reqmem = reqmem;
  man->cb_delmem = delmem;
}

static bool mman_reqmem(mman_t man, size_t size) {
  if (man->cb_reqmem == null) return false;
  if (size > SIZE_2M) return false;
  if (size > SIZE_4k) size = SIZE_2M;
  if (size < SIZE_4k) size = SIZE_4k;

  void *mem = man->cb_reqmem(null, size);
  if (mem == null) return false;

  allocarea_init(mem, size, null);
  man->size += size;

  void *ptr  = mem + 2 * sizeof(size_t);
  void *ptr2 = blk_split(ptr, sizeof(struct mman_pool));
  blk_setalloced(ptr, blk_size(ptr));
  blk_setfreed(ptr2, blk_size(ptr2));

  man->large_blk = freelist_put(man->large_blk, ptr2);
  return true;
}

static bool mman_delmem(mman_t man, mman_pool_t pool) {
  if (man->cb_delmem == null) return false;

  mman_pool_t prev = &man->main;
  for (; prev != null; prev = prev->next) {
    if (prev->next == pool) break;
  }
  if (prev == null) return false; // error

  prev->next = pool->next;
  man->cb_delmem(pool->ptr, allocarea_size(pool->ptr));

  return true;
}

dlexport void *mman_alloc(mman_t man, size_t size) {
  if (size == 0) size = 2 * sizeof(size_t);
  size = PADDING(size);

  void *ptr = freelists_match(man->freed, size);

  if (ptr == null) ptr = freelist_match(&man->large_blk, size);

  if (ptr == null) { // 不足就分配
    if (!mman_reqmem(man, size)) return null;
    ptr = freelist_match(&man->large_blk, size);
  }

  size_t realsize = blk_size(ptr);
  if (realsize >= size + 8 * sizeof(size_t)) {
    void *new_ptr = blk_split(ptr, size);
    blk_setfreed(new_ptr, blk_size(new_ptr));
    if (!freelists_put(man->freed, new_ptr)) {
      man->large_blk = freelist_put(man->large_blk, new_ptr);
    }
  }

  blk_setalloced(ptr, blk_size(ptr));
  man->alloced_size += blk_size(ptr);
  return ptr;
}

dlexport void mman_free(mman_t man, void *ptr) {
  if (ptr == null) return;

  man->alloced_size -= blk_size(ptr);

  ptr = blk_trymerge(ptr, (blk_detach_t)_detach, man);
  blk_setfreed(ptr, blk_size(ptr));
  if (!freelists_put(man->freed, ptr)) { //
    man->large_blk = freelist_put(man->large_blk, ptr);
  }
}

dlexport size_t mman_msize(mman_t man, void *ptr) {
  if (ptr == null) return 0;
  return blk_size(ptr);
}
