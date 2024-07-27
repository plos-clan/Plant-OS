#pragma once
#include <define.h>
#include <type.h>

// 标准库内存分配函数

dlimport void  *malloc(size_t size) __attr_malloc;
dlimport void  *xmalloc(size_t size);
dlimport void   free(void *ptr);
dlimport void  *calloc(size_t n, size_t size);
dlimport void  *realloc(void *ptr, size_t newsize);
dlimport void  *reallocarray(void *ptr, size_t n, size_t size);
dlimport void  *aligned_alloc(size_t align, size_t size);
dlimport size_t malloc_usable_size(void *ptr);
dlimport void  *memalign(size_t align, size_t size);
dlimport int    posix_memalign(void **memptr, size_t alignment, size_t size);
dlimport void  *pvalloc(size_t size);
dlimport void  *valloc(size_t size);

// 向系统请求内存
// 每次请求时 size 均为 page_size 的整数倍
typedef void *(*cb_reqmem_t)(size_t size);
// 将内存交还给系统
typedef void (*cb_delmem_t)(void *ptr, size_t size);

// freelist

//

/**
 *\brief 指定元素大小的内存池
 *
 */
typedef struct sized_mpool {
  void  *ptr;      // 指向内存区的指针
  size_t size;     // 内存区总大小
  size_t bsize;    // 每个元素的大小
  size_t len;      // 总共能容纳的元素个数
  size_t nalloced; // 已分配计数
  void  *freelist; // 空闲列表
} *sized_mpool_t;

/**
 *\brief 初始化一个内存池
 *
 *\param pool     param
 *\param ptr      param
 *\param bsize    池中元素的大小，必须大于等于 sizeof(size_t)
 *\param len      池中元素的个数
 */
dlexport void sized_mpool_init(sized_mpool_t pool, void *ptr, size_t bsize, size_t len);

/**
 *\brief 从内存池中分配
 *
 *\param pool     param
 *\return value
 */
dlexport void *sized_mpool_alloc(sized_mpool_t pool);

/**
 *\brief 
 *
 *\param pool     param
 *\param ptr      param
 */
dlexport void sized_mpool_free(sized_mpool_t pool, void *ptr);

/**
 *\brief 判断指定的内存地址是否在内存池中
 *
 *\param pool     内存池
 *\param ptr      要判断的地址
 *\return 指定的内存地址是否在内存池中
 */
dlexport bool sized_mpool_inpool(sized_mpool_t pool, void *ptr);

typedef struct mpool *mpool_t;

dlimport void  *mpool_setcb(mpool_t pool, cb_reqmem_t reqmem, cb_delmem_t delmem);
dlimport void  *mpool_alloc(mpool_t pool, size_t size);
dlimport void   mpool_free(mpool_t pool, void *ptr);
dlimport size_t mpool_msize(mpool_t pool, void *ptr); // 获取分配的内存的大小
