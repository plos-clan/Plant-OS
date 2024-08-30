#include <kernel.h>

void syscall_exit(int status) {
  extern mtask *mouse_use_task;
  mtask        *task = current_task();
  if (!(*(u8 *)(0xf0000000))) {
    klog("here\n");
    if (mouse_use_task == task) mouse_sleep(&mdec);
  }
  task_exit(status);
  infinite_loop;
  __builtin_unreachable();
}
