#include <fs.h>
#include <kernel.h>

void sb16_init();

struct MOUSE_DEC mdec;
size_t           memsize;
byte            *IVT;

void abort() {
  infinite_loop {
    asm_cli, asm_hlt;
  }
}

#if 0
#  define klogd(...) ((void)0)

void malloc_test() {
  size_t old_count = timerctl.count;
  klogw("%d", old_count);

#  define M 2048
#  define N (32 * 1024 * 1024)
  srand(114514);
  static void  *ptr_list[M];
  static size_t size_list[M];
  static u32    hash_list[M];
  klogi("alloc...");
  for (size_t i = 0; i < M; i++) {
    size_list[i] = rand() % 256;
    ptr_list[i]  = malloc(size_list[i]);
    // for (size_t j = 0; j < size_list[i]; j++) {
    //   ((u8 *)ptr_list[i])[j] = rand() >> 8;
    // }
    // hash_list[i] = memhash(ptr_list[i], size_list[i]);
    klogd("step 1: %d alloced: %p", i, ptr_list[i]);
  }
  klogi("test...");
  for (size_t i = 0; i < N; i++) {
    size_t n = rand() % M;
    // size_t hash = memhash(ptr_list[n], size_list[n]);
    // if (hash_list[n] != hash) {
    //   info("step 2: %d:%d check: %p", i, n, ptr_list[n]);
    //   fatal("hash error: %u != %u", hash, hash_list[n]);
    // }
    free(ptr_list[n]);
    klogd("step 2: %d:%d freed: %p", i, n, ptr_list[n]);
    size_list[n] = rand() % 256;
    ptr_list[n]  = malloc(size_list[n]);
    // for (size_t j = 0; j < size_list[n]; j++) {
    //   ((u8 *)ptr_list[n])[j] = rand() >> 8;
    // }
    // hash_list[n] = memhash(ptr_list[n], size_list[n]);
    klogd("step 2: %d:%d alloced: %p", i, n, ptr_list[n]);
    if (i % (N / 100) == 0) klogi("  %d%%", i / (N / 100));
  }
  klogi("free...");
  for (size_t i = 0; i < M; i++) {
    // size_t hash = memhash(ptr_list[i], size_list[i]);
    // if (hash_list[i] != hash) {
    //   klogd("step 3: %d check: %p", i, ptr_list[i]);
    //   fatal("hash error: %u != %u", hash, hash_list[i]);
    // }
    free(ptr_list[i]);
    klogd("step 3: %d freed: %p", i, ptr_list[i]);
  }

  size_t new_count = timerctl.count;
  klogw("%d", new_count);
  klogw("%d", new_count - old_count);

  fatal("test end.");
}
#endif

#ifdef __clang__
#  pragma clang optimize off
#endif
void *pci_addr_base;
void sysinit() {
  do_init_seg_register();
  memsize = memtest(0x00400000, 0xbfffffff);
  init_page();
  IVT = page_malloc(0x10000);
  memcpy(IVT, null, 0x10000); // 这是正确的，忽略这个 warning
  // init_gdtidt();
  init_pic();

  asm_sti;
  irq_mask_clear(0);
  irq_mask_clear(1);
  asm_set_em, asm_set_ts, asm_set_ne;

  memory_init(page_malloc(128 * 1024 * 1024), 128 * 1024 * 1024);

  // 可以 setmode

  init_pit();
  init_tty();
  screen_clear();
  sb16_init();
  vdisk_init();
  vfs_init();

  if (memsize < 256 * 1024 * 1024) {
    fatal("You should have at least 256MB memory in your pc to start Plant-OS.");
  } else {
    info("the memory test has been passed! Your PC has %dMB memory", memsize / (1024 * 1024));
  }
  pci_addr_base = page_malloc(1 * 1024 * 1024);
  init_pci(pci_addr_base);
  // malloc_test();

  init_keyboard();
  into_mtask();
  infinite_loop;
}
#ifdef __clang__
#  pragma clang optimize on
#endif
