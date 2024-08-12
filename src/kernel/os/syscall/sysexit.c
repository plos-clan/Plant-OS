#include <kernel.h>

void syscall_exit(int status) {
  extern mtask *mouse_use_task;
  mtask        *task = current_task();
  if (!(*(u8 *)(0xf0000000))) {
    klog("here\n");
    //  list_free_with(vfs_now->path, free);
    if (mouse_use_task == task) { mouse_sleep(&mdec); }
    // free(vfs_now->cache);
    // free((void *)vfs_now);
  } else {
    // get_task(task->ptid)->nfs = task->nfs;
  }
  task_exit(status);
  while (1) {}
  __builtin_unreachable();
}
