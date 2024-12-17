#include <kernel.h>

extern task_t mouse_use_task;

void syscall_exit(int status) {
  task_exit(status);
  infinite_loop;
  __builtin_unreachable();
}
