#include <kernel.h>

extern task_t mouse_use_task;

void syscall_exit(int status) {
  // if (!(*(u8 *)(0xf0000000))) {
  //   klog("here\n");
  //   if (mouse_use_task == mouse_use_task) mouse_sleep(&mdec);
  // }
  task_exit(status);
  infinite_loop;
  __builtin_unreachable();
}
