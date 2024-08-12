#include <kernel.h>

void kernel_main() {
  sysinit();
  infinite_loop;
}
