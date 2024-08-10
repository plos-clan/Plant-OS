#include <kernel.h>

extern u8 *IVT;

#pragma GCC optimize("O0")

#pragma clang optimize off

void asm16_int(u8 intnum, regs16 *regs) {
  asm_set_cr3(PDE_ADDRESS);
  memcpy(null, IVT, 0x1000);
  int32(intnum, regs);
  asm_set_cr3(current_task()->pde);
  asm_sti;
}
