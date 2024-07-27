#include "kernel/8295a.h"
#include "kernel/timer.h"
#include "kernel/vdisk.h"
#include <kernel.h>

int              init_ok_flag = 0;
struct MOUSE_DEC mdec;
memory          *public_heap;
u32              memsize;
u8              *IVT;

void abort() {
  for (;;) {
    asm_cli;
    asm_hlt;
  }
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
  void *heap  = page_malloc(128 * 1024 * 1024);
  public_heap = memory_init((u32)heap, 128 * 1024 * 1024);

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
