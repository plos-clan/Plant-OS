// This code is released under the MIT License

#include <libc.h>

static struct mman mman;

dlexport void *malloc(size_t size) {
  return mman_alloc(&mman, size);
}

dlexport void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == null) exit(1);
  return ptr;
}

dlexport void free(void *ptr) {
  mman_free(&mman, ptr);
}

dlexport void *calloc(size_t n, size_t size) {
  if (__builtin_mul_overflow(n, size, &size)) return null;
  void *ptr = malloc(size);
  if (ptr == null) return null;
  memset(ptr, 0, size);
  return ptr;
}

dlexport void *realloc(void *ptr, size_t newsize) {
  return mman_realloc(&mman, ptr, newsize);
}

dlexport void *reallocarray(void *ptr, size_t n, size_t size) {
  return realloc(ptr, n * size);
}

dlexport void *aligned_alloc(size_t align, size_t size) {
  return mman_aligned_alloc(&mman, size, align);
}

dlexport size_t malloc_usable_size(void *ptr) {
  return mman_msize(&mman, ptr);
}

dlexport void *memalign(size_t align, size_t size) {
  return mman_aligned_alloc(&mman, size, align);
}

dlexport int posix_memalign(void **memptr, size_t alignment, size_t size) {
  void *ptr = mman_aligned_alloc(&mman, size, alignment);
  if (ptr == null) return 1;
  *memptr = ptr;
  return 0;
}

dlexport void *pvalloc(size_t size) {
  return aligned_alloc(4096, size);
}

dlexport void *valloc(size_t size) {
  return aligned_alloc(4096, size);
}

static void *mmap_buf_4k[4] = {null};
static void *mmap_buf_2M    = null;

static void *wrap_mmap(void *addr, size_t size) {
  if (size == SIZE_2M) {
    if (mmap_buf_2M != null) {
      void *tmp   = mmap_buf_2M;
      mmap_buf_2M = null;
      memset(tmp, 0, SIZE_2M);
      return tmp;
    }
    return mmap(addr, size);
  }
  if (size == SIZE_4k) {
#pragma unroll
    for (int i = 0; i < 4; i++) {
      if (mmap_buf_4k[i] != null) {
        void *tmp      = mmap_buf_4k[i];
        mmap_buf_4k[i] = null;
        memset(tmp, 0, SIZE_4k);
        return tmp;
      }
    }
    return mmap(addr, size);
  }
  return mmap(addr, size);
}

static void wrap_munmap(void *addr, size_t size) {
  if (size == SIZE_2M) {
    if (mmap_buf_2M == null) {
      mmap_buf_2M = addr;
      return;
    }
    munmap(addr, size);
    return;
  }
  if (size == SIZE_4k) {
#pragma unroll
    for (int i = 0; i < 4; i++) {
      if (mmap_buf_4k[i] == null) {
        mmap_buf_4k[i] = addr;
        return;
      }
    }
    munmap(addr, size);
    return;
  }
  munmap(addr, size);
}

bool __libc_init_mman() {
  void *ptr = mmap(null, SIZE_4k);
  if (ptr == null) return false;
  mman_init(&mman, ptr, SIZE_4k);
  mman_setcb(&mman, wrap_mmap, wrap_munmap);
  return true;
}
