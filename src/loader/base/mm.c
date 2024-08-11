#include "libc-base/string/mem.h"
#include <loader.h>

#define EFLAGS_AC_BIT     0x00040000
#define CR0_CACHE_DISABLE 0x60000000

u32 memtest(u32 start, u32 end) {
  char flg486 = 0;
  u32  eflg, cr0, i;

  /* 确认CPU是386还是486以上的 */
  eflg  = asm_get_flags();
  eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
  asm_set_flags(eflg);
  eflg = asm_get_flags();
  if ((eflg & EFLAGS_AC_BIT) != 0) {
    /* 如果是386，即使设定AC=1，AC的值还会自动回到0 */
    flg486 = 1;
  }

  eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
  asm_set_flags(eflg);

  if (flg486 != 0) {
    cr0  = asm_get_cr0();
    cr0 |= CR0_CACHE_DISABLE; /* 禁止缓存 */
    asm_set_cr0(cr0);
  }

  i = memtest_sub(start, end);

  if (flg486 != 0) {
    cr0  = asm_get_cr0();
    cr0 &= ~CR0_CACHE_DISABLE; /* 允许缓存 */
    asm_set_cr0(cr0);
  }

  return i;
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

void *page_malloc(int size) {
  void *p = mpool_alloc(&pool, size);
  bzero(p, size);
  return p;
}

void page_free(void *p, int size) {
  mpool_free(&pool, p);
}
