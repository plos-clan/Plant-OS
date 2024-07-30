#include <kernel.h>

void syscall_exit(int status) {
  task_exit(status);
  while (1) {}
  __builtin_unreachable();
}
