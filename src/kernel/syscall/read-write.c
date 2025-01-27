#include <kernel.h>

// enum {
//   read,
//   write,
//   seek,
//   open,
//   close,
// };
int syscall_open(const char *path, int flags, int mode) {
  if (!check_string_permission(path)) task_abort();
  return resman_open(current_task->resman, current_task->working_dir, path);
}

isize syscall_read(int fd, void *addr, usize size) {
  if (fd < 0) return -1;
  if (addr == null) return -1;
  describtor_t file = avltree_get(current_task->resman->avltree, fd);
  if (file == null) return -1;
  return vfs_read(file->node, addr, 0, size);
}

isize syscall_write(int fd, const void *addr, usize size) {
  if (fd < 0) return -1;
  if (addr == null) return -1;
  return 0;
}
