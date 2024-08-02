#include <libc.h>

dlimport bool __libc_init_mman();

dlimport int main();

void _start() {
  __libc_init_mman();

  int ret = main();

  exit(ret);
}
