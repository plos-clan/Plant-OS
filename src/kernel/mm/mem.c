#include <kernel.h>

#define EFLAGS_AC_BIT ((size_t)0x00040000)

size_t memtest_sub(size_t, size_t);

size_t memtest(size_t start, size_t end) {
  bool flg486 = false;

  // 确认CPU是386还是486以上的
  asm_set_flags(asm_get_flags() | EFLAGS_AC_BIT);
  // 如果是386，即使设定AC=1，AC的值还会自动回到0
  if (asm_get_flags() & EFLAGS_AC_BIT) {
    flg486 = true;
    asm_set_flags(asm_get_flags() & ~EFLAGS_AC_BIT);
  }

  if (flg486) asm_set_cd, asm_set_nw;

  size_t size = memtest_sub(start, end);

  if (flg486) asm_clr_nw, asm_clr_cd;

  return size;
}

static struct mpool pool;

void memory_init(void *ptr, u32 size) {
  mpool_init(&pool, ptr, size);
}

void *malloc(size_t size) {
  void *ptr = mpool_alloc(&pool, size);
  // klogd("alloc %-10p %d -> %d", ptr, size, mpool_msize(&pool, ptr));
  return ptr;
}

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == null) abort();
  return ptr;
}

void free(void *ptr) {
  // klogd("free  %-10p %d", ptr, mpool_msize(&pool, ptr));
  mpool_free(&pool, ptr);
}

void *calloc(size_t n, size_t size) {
  if (__builtin_mul_overflow(n, size, &size)) return null;
  void *ptr = malloc(size);
  if (ptr == null) return null;
  memset(ptr, 0, size);
  return ptr;
}

void *realloc(void *ptr, size_t size) {
  // void *old_ptr = ptr;
  ptr = mpool_realloc(&pool, ptr, size);
  // klogd("realloc %-10p %d -> %-10p %d", old_ptr, mpool_msize(&pool, ptr), ptr, size);
  return ptr;
}

void *reallocarray(void *ptr, size_t n, size_t size) {
  return realloc(ptr, n * size);
}

void *aligned_alloc(size_t align, size_t size) {
  return mpool_aligned_alloc(&pool, size, align);
}

size_t malloc_usable_size(void *ptr) {
  return mpool_msize(&pool, ptr);
}

void *memalign(size_t align, size_t size) {
  return mpool_aligned_alloc(&pool, size, align);
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
  void *ptr = mpool_aligned_alloc(&pool, size, alignment);
  if (ptr == null) return 1;
  *memptr = ptr;
  return 0;
}

void *pvalloc(size_t size) {
  return aligned_alloc(4096, size);
}

void *valloc(size_t size) {
  return aligned_alloc(4096, size);
}
