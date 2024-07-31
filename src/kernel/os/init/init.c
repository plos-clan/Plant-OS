#include "kernel/8295a.h"
#include "kernel/timer.h"
#include "kernel/vdisk.h"
#include <kernel.h>
void             disable_sb16();
int              init_ok_flag = 0;
struct MOUSE_DEC mdec;
size_t           memsize;
u8              *IVT;

void abort() {
  while (true) {
    asm_cli;
    asm_hlt;
  }
}

#if 1
static unsigned int rand_seed = 1;

int rand() {
  rand_seed ^= rand_seed << 13;
  rand_seed ^= rand_seed >> 17;
  rand_seed ^= rand_seed << 5;
  return rand_seed & INT32_MAX;
}

void srand(unsigned int seed) {
  rand_seed = seed;
}

void malloc_test() {
  size_t old_count = timerctl.count;
  logw("%d", old_count);

#  define M 2048
#  define N (32 * 1024)
  srand(114514);
  static void  *ptr_list[M];
  static size_t size_list[M];
  static u32    hash_list[M];
  for (size_t i = 0; i < M; i++) {
    size_list[i] = rand() % 256;
    ptr_list[i]  = malloc(size_list[i]);
    for (size_t j = 0; j < size_list[i]; j++) {
      ((u8 *)ptr_list[i])[j] = rand() >> 8;
    }
    hash_list[i] = memhash(ptr_list[i], size_list[i]);
    logd("step 1: %d alloced: %p", i, ptr_list[i]);
  }
  for (size_t i = 0; i < N; i++) {
    size_t n    = rand() % M;
    size_t hash = memhash(ptr_list[n], size_list[n]);
    if (hash_list[n] != hash) {
      info("step 2: %d:%d check: %p", i, n, ptr_list[n]);
      fatal("hash error: %u != %u", hash, hash_list[n]);
    }
    free(ptr_list[n]);
    logd("step 2: %d:%d freed: %p", i, n, ptr_list[n]);
    size_list[n] = rand() % 256;
    ptr_list[n]  = malloc(size_list[n]);
    for (size_t j = 0; j < size_list[n]; j++) {
      ((u8 *)ptr_list[n])[j] = rand() >> 8;
    }
    hash_list[n] = memhash(ptr_list[n], size_list[n]);
    logd("step 2: %d:%d alloced: %p", i, n, ptr_list[n]);
  }
  for (size_t i = 0; i < M; i++) {
    size_t hash = memhash(ptr_list[i], size_list[i]);
    if (hash_list[i] != hash) {
      logd("step 3: %d check: %p", i, ptr_list[i]);
      fatal("hash error: %u != %u", hash, hash_list[i]);
    }
    free(ptr_list[i]);
    logd("step 3: %d freed: %p", i, ptr_list[i]);
  }

  size_t new_count = timerctl.count;
  logw("%d", new_count);
  logw("%d", new_count - old_count);

  fatal("test end.");
}
#endif

#ifdef __clang__
#  pragma clang optimize off
#endif
void sysinit() {
  do_init_seg_register();
  memsize = memtest(0x00400000, 0xbfffffff);
  init_page();
  IVT = page_malloc_one_no_mark();
  memcpy(IVT, null, 0x1000);
  init_gdtidt();
  init_pic();
  asm_sti;
  irq_mask_clear(0);
  irq_mask_clear(1);
  asm_set_em, asm_set_ts, asm_set_ne;
  memory_init(page_malloc(128 * 1024 * 1024), 128 * 1024 * 1024);
  init_pit();
  init_tty();
  clear();
  disable_sb16();
  init_vdisk();
  init_vfs();
  register_fat();

  if (memsize / (1024 * 1024) < 256) {
    fatal("You should have at least 256MB memory in your pc to start Plant-OS.");
  } else {
    info("the memory test has been passed Your PC has %dMB memory", memsize / (1024 * 1024));
  }

  // malloc_test();

  init_keyboard();
  into_mtask();
  while (true) {}
}
#ifdef __clang__
#  pragma clang optimize on
#endif
