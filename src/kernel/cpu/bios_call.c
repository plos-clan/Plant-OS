#include <kernel.h>

extern byte *IVT;

#pragma GCC optimize("O0")

#pragma clang optimize off

void asm16_int(u8 intnum, regs16 *regs) {
  asm_set_cr3(PDE_ADDRESS);
  memcpy(null, IVT, PAGE_SIZE); // 这是正确的，忽略这个 warning
  int32(intnum, regs);
  asm_set_cr3(current_task()->pde);
  init_pic();
  asm_sti;
}
