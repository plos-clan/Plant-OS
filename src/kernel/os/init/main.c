#include <define.h>
#include <kernel/os.h>
#include <type.h>
void kernel_main() {
  sysinit();
  *(u8 *)(0xb8000) = 'K';
  *(u8 *)(0xb8001) = 0x0c;
  for (;;)
    ;
}