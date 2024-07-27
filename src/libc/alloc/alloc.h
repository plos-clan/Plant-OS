#pragma once
#include <libc.h>

#include "block.h"
#include "freelist.h"

// 内存分配区的块结构

typedef struct allocator {
  size_t size;
} *allocator_t;

void   allocator_init(allocator_t alloc, size_t size);
bool   allocator_deinit(allocator_t alloc);
void  *allocator_alloc(allocator_t alloc, size_t size);
size_t allocator_msize(allocator_t alloc, void *ptr);
void   allocator_free(allocator_t alloc, void *ptr);
void   allocator_freeall(allocator_t alloc);

//

typedef struct alloc_area {
  void  *ptr;          // 指向分配区的指针
  size_t size;         // 分配区的大小
  size_t alloced_size; // 已分配内存的大小
  void **freeed;       // 空闲链表
} *alloc_area_t;

void alloc_area_init(alloc_area_t area, void *userptr) {
  size_t *ptr  = area->ptr;
  size_t  len  = area->size / sizeof(size_t);
  auto    data = (size_t)area | AREA_FLAG | (area->size == 2 * 1024 * 1024 ? SIZE_FLAG : 0);
  ptr[0] = ptr[len - 1] = data; // 将首尾设为占位符
  blk_setsize(ptr + 2, area->size - 4 * sizeof(size_t));
}

size_t alloc_area_msize(alloc_area_t area, void *ptr) {
  return blk_size(ptr);
}

void *alloc_area_alloc(alloc_area_t area, size_t size) {}

void alloc_area_free(alloc_area_t area, void *ptr) {
  ptr = blk_trymerge(ptr);
}

void *mpool_single_alloc() {}

typedef struct {
  spin_t       spin;
  size_t       page_size;
  alloc_area_t main_area;
  cb_reqmem_t  cb_reqmem;
  cb_delmem_t  cb_delmem;
} *mpool_t;

static void *sys_reqqmem(mpool_single_t pool, size_t size) {
  if (pool->cb_reqmem == null) return null;
  return pool->cb_reqmem((size + pool->page_size - 1) & ~(pool->page_size - 1));
}

typedef struct mman {
} mman_t;

#define afrray_len 16
typedef struct afarray {
  struct {
    size_t size; // 块大小
    void  *ptr;  // 块开始处指针
    void  *end;  // 块结束处指针
  } data[afrray_len];
  struct afarray *next; // 下一项
} *afarray_t;

dlimport void  *mpool_alloc(mpool_t *pool, size_t size);
dlimport void   mpool_free(mpool_t *pool, void *ptr);
dlimport size_t mpool_msize(mpool_t *pool, void *ptr); // 获取分配的内存的大小
dlimport size_t mpool_total_size(mpool_t *pool);       // 占用的内存总大小
dlimport size_t mpool_alloced_size(mpool_t *pool);     // 分配的内存总大小

dlexport void *malloc(size_t size) {
  return null;
}
