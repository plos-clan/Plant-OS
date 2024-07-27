#include <kernel.h>
#include <cpu.h>

void INT(u8 intnum, regs16_t *regs) {
  extern u8                 *IVT;
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
  // logk("%08x\n",current_task()->pde);
  set_segmdesc(gdt + 1000, 0xffffffff, 0, AR_CODE32_ER); // CODE32
  set_segmdesc(gdt + 1001, 0xfffff, 0, AR_CODE16_ER);    // CODE16
  set_segmdesc(gdt + 1002, 0xfffff, 0, AR_DATA16_RW);    // DATA16
  //memcpy(0, IVT, 0x400);
  int32(intnum, regs);
  set_segmdesc(gdt + 1000, 0, 0, 0); // 临时GDT清零
  set_segmdesc(gdt + 1001, 0, 0, 0);
  set_segmdesc(gdt + 1002, 0, 0, 0);

  set_cr3(current_task()->pde);
  asm_sti;
}
