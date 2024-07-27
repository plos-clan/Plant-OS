#pragma once
#include <define.h>
#include <type.h>

dlimport void  *malloc(size_t size);
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
