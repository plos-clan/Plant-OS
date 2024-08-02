#include <libc.h>

static struct mman mman;

void *malloc(size_t size) {
  return mman_alloc(&mman, size);
}

void free(void *ptr) {
  mman_free(&mman, ptr);
}

bool __libc_init_mman() {
  void *ptr = mmap(null, SIZE_4k);
  if (ptr == null) return false;
  mman_init(&mman, ptr, SIZE_4k);
  mman_setcb(&mman, mmap, munmap);
  return true;
}
