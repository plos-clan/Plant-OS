#include "base/string/mem.h"
#include <kernel.h>

extern u8 *IVT;

#ifdef __clang__
#  pragma clang optimize off
#endif
void INT(u8 intnum, regs16_t *regs) {
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
  // logd("%08x\n",current_task()->pde);
  asm_set_cr3(PDE_ADDRESS);
  set_segmdesc(gdt + 1000, 0xffffffff, 0, AR_CODE32_ER); // CODE32
  set_segmdesc(gdt + 1001, 0xfffff, 0, AR_CODE16_ER);    // CODE16
  set_segmdesc(gdt + 1002, 0xfffff, 0, AR_DATA16_RW);    // DATA16
  memcpy(null, IVT, 0x1000);
  int32(intnum, regs);
  set_segmdesc(gdt + 1000, 0, 0, 0); // 临时GDT清零
  set_segmdesc(gdt + 1001, 0, 0, 0);
  set_segmdesc(gdt + 1002, 0, 0, 0);

  asm_set_cr3(current_task()->pde);
  asm_sti;
}
#ifdef __clang__
#  pragma clang optimize on
#endif
