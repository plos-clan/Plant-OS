#include <kernel.h>

#define EFLAGS_AC_BIT     0x00040000
#define CR0_CACHE_DISABLE 0x60000000

u32 memtest_sub(u32, u32);

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

void *realloc(void *ptr, u32 size) {
  void *new = malloc(size);
  if (new == null || ptr == null) return new;
  memcpy(new, ptr, mpool_msize(&pool, ptr));
  free(ptr);
  return new;
}

void *kmalloc(int size) {
  return malloc(size);
  // void *p;
  // p = page_malloc(size + sizeof(int));
  // if (p == NULL) return NULL;
  // *(int *)p = size;
  // return (char *)p + sizeof(int);
}

void kfree(void *p) {
  free(p);
  // if (p == NULL) return;
  // int size = *(int *)(p - sizeof(int));
  // page_free((char *)p - sizeof(int), size + sizeof(int));
}

void *krealloc(void *ptr, u32 size) {
  void *new = kmalloc(size);
  if (new == null || ptr == null) return new;
  memcpy(new, ptr, mpool_msize(&pool, ptr));
  kfree(ptr);
  // void *new = kmalloc(size);
  // if (ptr) {
  //   memcpy(new, ptr, *(int *)((int)ptr - 4));
  //   kfree(ptr);
  // }
  return new;
}