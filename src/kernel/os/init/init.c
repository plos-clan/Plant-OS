#include "kernel/8295a.h"
#include "kernel/timer.h"
#include "kernel/vdisk.h"
#include <kernel.h>

int              init_ok_flag = 0;
struct MOUSE_DEC mdec;
u32              memsize;
u8              *IVT;

void abort() {
  for (;;) {
    asm_cli;
    asm_hlt;
  }
}

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

#ifdef __clang__
#  pragma clang optimize off
#endif
void sysinit() {
  do_init_seg_register();
  init_page();
  IVT = page_malloc_one_no_mark();
  memcpy(IVT, null, 0x1000);
  init_gdtidt();
  init_pic();
  asm_sti;
  irq_mask_clear(0);
  irq_mask_clear(1);
  asm_set_cr0(asm_get_cr0() | CR0_EM | CR0_TS | CR0_NE);
  void *heap = page_malloc(128 * 1024 * 1024);
  memory_init(heap, 128 * 1024 * 1024);

  // size_t old_count = timerctl.count;
  // logw("%d", old_count);

  // #define M 2048
  // #define N (32 * 1024 * 1024)
  //   srand(114514);
  //   static void *ptr_list[M];
  //   for (size_t i = 0; i < M; i++) {
  //     ptr_list[i] = malloc(rand() % 256);
  //     // logd("step 1: %d alloced: %p", i, ptr_list[i]);
  //   }
  //   for (size_t i = 0; i < N; i++) {
  //     size_t n = rand() % M;
  //     free(ptr_list[n]);
  //     // logd("step 2: %d:%d freed: %p", i, n, ptr_list[n]);
  //     ptr_list[n] = malloc(rand() % 256);
  //     // logd("step 2: %d:%d alloced: %p", i, n, ptr_list[n]);
  //     if (ptr_list[n] != null && ptr_list[n] < heap || ptr_list[n] > heap + 128 * 1024 * 1024) {
  //       fatal();
  //     }
  //   }
  //   for (size_t i = 0; i < M; i++) {
  //     free(ptr_list[i]);
  //     // logd("step 3: %d freed: %p", i, ptr_list[i]);
  //   }

  // size_t new_count = timerctl.count;
  // logw("%d", new_count);
  // fatal("%d", new_count - old_count);

  init_pit();
  init_tty();
  clear();
  init_vdisk();
  init_vfs();
  register_fat();
  memsize = memtest(0x00400000, 0xbfffffff);

  if (memsize / (1024 * 1024) < 256) {
    fatal("You should have at least 256MB memory in your pc to start Plant-OS.");
  } else {
    info("the memory test has been passed Your PC has %dMB memory", memsize / (1024 * 1024));
  }
  init_keyboard();
  into_mtask();
  while (true) {}
}
#ifdef __clang__
#  pragma clang optimize on
#endif
