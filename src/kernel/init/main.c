#include <kernel.h>

void asm_sysenter_handler();

void kernel_main() {
  sysinit();
  asm_wrmsr(IA32_SYSENTER_EIP, asm_sysenter_handler, 0);
  into_mtask();
  infinite_loop {
    klogf("kernel_main error");
    abort();
  }
}
