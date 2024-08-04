#include <libc.h>

static struct mman mman;

void *malloc(size_t size) {
  return mman_alloc(&mman, size);
}

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == null) exit(1);
  return ptr;
}

void free(void *ptr) {
  mman_free(&mman, ptr);
}

void *calloc(size_t n, size_t size) {
  void *ptr = malloc(n * size);
  if (ptr == null) return null;
  memset(ptr, 0, n * size);
  return ptr;
}

void *realloc(void *ptr, size_t newsize) {
  void *newptr = malloc(newsize);
  if (newptr == null) return null;
  size_t size = malloc_usable_size(ptr);
  memcpy(newptr, ptr, size);
  free(ptr);
  return newptr;
}

void *reallocarray(void *ptr, size_t n, size_t size) {
  return realloc(ptr, n * size);
}

void *aligned_alloc(size_t align, size_t size);

size_t malloc_usable_size(void *ptr) {
  return mman_msize(&mman, ptr);
}

void *memalign(size_t align, size_t size);
int   posix_memalign(void **memptr, size_t alignment, size_t size);
void *pvalloc(size_t size);
void *valloc(size_t size);

bool __libc_init_mman() {
  void *ptr = mmap(null, SIZE_4k);
  if (ptr == null) return false;
  mman_init(&mman, ptr, SIZE_4k);
  mman_setcb(&mman, mmap, munmap);
  return true;
}
