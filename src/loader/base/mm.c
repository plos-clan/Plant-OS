#include <loader.h>

#define EFLAGS_AC_BIT 0x00040000

u32 memtest(u32 start, u32 end) {
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

void memman_init(void *ptr, size_t size) {
  mpool_init(&pool, ptr, size);
}

void *malloc(size_t size) {
  void *p = mpool_alloc(&pool, size);
  bzero(p, size);
  return p;
}

void free(void *ptr) {
  mpool_free(&pool, ptr);
}

void *realloc(void *ptr, size_t size) {
  return mpool_realloc(&pool, ptr, size);
}

void *page_alloc(int size) {
  void *p = mpool_alloc(&pool, size);
  bzero(p, size);
  return p;
}

void page_free(void *p, int size) {
  mpool_free(&pool, p);
}
