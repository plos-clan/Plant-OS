#include "kernel/tty.h"
#include <kernel.h>

int              init_ok_flag = 0;
struct MOUSE_DEC mdec;
memory          *public_heap;
u32              memsize;

void sysinit() {
  do_init_seg_register();
  init_page();
  init_gdtidt();
  init_pic();
  asm_sti;
  irq_mask_clear(0);
  set_cr0(get_cr0() | CR0_EM | CR0_TS | CR0_NE);
  void *heap  = page_malloc(128 * 1024 * 1024);
  public_heap = memory_init((u32)heap, 128 * 1024 * 1024);
  init_pit();
  init_tty();
  print("\e[1;32mcount of mines:\e[m\n");
  into_mtask();

  while (true) {}
}
