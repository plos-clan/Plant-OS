#include <kernel.h>

#define EFLAGS_AC_BIT 0x00040000

u32 memtest_sub(u32, u32);

u32 memtest(u32 start, u32 end) {
  bool flg486 = false;

  // 确认CPU是386还是486以上的
  asm_set_flags(asm_get_flags() | EFLAGS_AC_BIT);
  // 如果是386，即使设定AC=1，AC的值还会自动回到0
  if (asm_get_flags() & EFLAGS_AC_BIT) {
    flg486 = true;
    asm_set_flags(asm_get_flags() & ~EFLAGS_AC_BIT);
  }

  if (flg486) asm_set_cd;

  size_t size = memtest_sub(start, end);

  if (flg486) asm_clr_cd;

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

void free(void *ptr) {
  // klogd("free  %-10p %d", ptr, mpool_msize(&pool, ptr));
  mpool_free(&pool, ptr);
}

size_t malloc_usable_size(void *ptr) {
  return mpool_msize(&pool, ptr);
}

void *realloc(void *ptr, size_t size) {
  return mpool_realloc(&pool, ptr, size);
}

// 老旧代码
// void *kmalloc(int size) {
//   return malloc(size);
// }
// void kfree(void *p) {
//   free(p);
// }
// void *krealloc(void *ptr, u32 size) {
//   return realloc(ptr, size);
// }
