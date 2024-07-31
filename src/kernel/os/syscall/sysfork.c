#include <kernel.h>

int syscall_fork() {
  return task_fork();
}