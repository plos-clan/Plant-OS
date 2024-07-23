#include "kernel/8295a.h"
#include "kernel/page.h"
#include "libc/asm.h"
#include "loader/dosldr/8295a.h"
#include <kernel.h>
int              init_ok_flag = 0;
struct MOUSE_DEC mdec;
memory          *public_heap;
u32              memsize;
void             sysinit() {
  do_init_seg_register();
  init_page();
  init_gdtidt();
  init_pic();
  asm_sti;
  irq_mask_clear(0);
  set_cr0(get_cr0() | CR0_EM | CR0_TS | CR0_NE);
  init_pit();
  into_mtask();
  for (;;)
    ;
}