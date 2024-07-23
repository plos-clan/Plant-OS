#pragma once
#include <libc.h>

// 向系统请求内存
// 每次请求时 size 均为 page_size 的整数倍
typedef void *(*cb_reqmem_t)(size_t size);
// 将内存交还给系统
typedef void (*cb_delmem_t)(void *ptr, size_t size);

typedef struct alloc_area {
  void  *ptr;  // 指向分配区的指针
  size_t size; // 分配区的大小
} *alloc_area_t;

void alloc_area_init(alloc_area_t area) {
  size_t *ptr = area->ptr;
  size_t  len = area->size / sizeof(size_t);
  // 将首尾设为占位符
  ptr[0]       = SIZE_MAX;
  ptr[len - 1] = SIZE_MAX;
  // 初始化为单一未分配块
  size_t size  = area->size - 2 * sizeof(size_t);
  ptr[1]       = size | 1; // 标记为未分配
  ptr[len - 2] = size | 1; // 标记为未分配
}

size_t alloc_area_msize(alloc_area_t area, void *ptr) {
  return *((size_t *)ptr - 1);
}

void *alloc_area_alloc(alloc_area_t area, size_t size) {}

void alloc_area_free(alloc_area_t area, void *ptr) {
  size_t size = alloc_area_msize(area, ptr);
}

// 单一分配区的内存池
typedef struct {
  spin_t       spin;
  size_t       page_size; // 页面大小
  alloc_area_t main_area; // 主分配区
  cb_reqmem_t  cb_reqmem;
  cb_delmem_t  cb_delmem;
} *mpool_single_t;

// 多个分配区的内存池
typedef struct {
  spin_t       spin;
  size_t       page_size; // 页面大小 (必须为 2 的幂)
  alloc_area_t main_area; // 主分配区
  cb_reqmem_t  cb_reqmem;
  cb_delmem_t  cb_delmem;
} *mpool_t;

static void *sys_reqqmem(mpool_single_t pool, size_t size) {
  if (pool->cb_reqmem == null) return null;
  return pool->cb_reqmem((size + pool->page_size - 1) & ~(pool->page_size - 1));
}

typedef struct alist {
  size_t        size; // 块大小
  struct alist *next; // 下一项
  void         *ptr;  // 块开始处指针
  void         *end;  // 块结束处指针
} *alist_t;

typedef struct flist {
  size_t        size; // 块大小
  struct flist *next; // 下一项
  struct flist *prev; // 上一项
} *flist_t;

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
