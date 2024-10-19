#include <kernel.h>

extern byte *IVT;

#pragma GCC optimize("O0")

#pragma clang optimize off

void asm_int32(byte intnum, regs16 *regs);

void asm16_int(byte intnum, regs16 *regs) {
  var flag = asm_get_flags();
  asm_set_cr3(PDE_ADDRESS);
  memcpy(null, IVT, PAGE_SIZE); // 这是正确的，忽略这个 warning
  asm_int32(intnum, regs);
  asm_set_cr3(current_task()->pde);
  init_pic();
  asm_set_flags(flag);
}
