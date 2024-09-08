#include <kernel.h>

void kernel_main() {
  sysinit();
  into_mtask();
  infinite_loop;
}
