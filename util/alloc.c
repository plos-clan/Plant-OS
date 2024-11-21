#include <libc-base.h>

static struct mpool pool;

dlexport void allocator_init(void *ptr, size_t size) {
  mpool_init(&pool, ptr, size);
}

dlexport void *malloc(size_t size) {
  return mpool_alloc(&pool, size);
}

dlexport void free(void *ptr) {
  mpool_free(&pool, ptr);
}
