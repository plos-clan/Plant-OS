#include <kernel.h>

// enum {
//   read,
//   write,
//   seek,
//   open,
//   close,
// };

//TODO: The parameter mode does not support yet
int syscall_open(const char *path, int flags, int mode) {
  if (!check_string_permission(path)) task_abort();
  int fd = resman_open(current_task->resman, current_task->working_dir, path);
  if (fd < 0) return -1;

  handle_t file = avltree_get(current_task->resman->avltree, fd);

  // 检查文件句柄是否合法
  if (file == null) return -1;
  if (file->fd == null) return -1;
  if (file->fd->node == null) return -1;
  if (file->fd->node->type == file_dir) {
    resman_close(current_task->resman, fd);
    return -1;
  }

  return fd;
}

isize syscall_read(int fd, void *addr, usize size) {
  if (fd < 0) return -1;
  if (addr == null) return -1;

  handle_t file = avltree_get(current_task->resman->avltree, fd);

  // 检查文件句柄是否合法
  if (file == null) return -1;
  if (file->fd == null) return -1;
  if (file->fd->node == null) return -1;
  if (file->fd->node->type == file_dir) return -1;

  klogd("read %s\n", file->fd->node->name);
  klogd("read %p\n", file->fd->node->handle);
  klogd("read %d\n", file->fd->node->fsid);

  isize read_bytes = vfs_read(file->fd->node, addr, file->offset, size);
  klogd("read_bytes %d\n", read_bytes);
  // 这里offset必不可能超过文件大小
  file->offset += read_bytes;

  return read_bytes;
}

isize syscall_write(int fd, const void *addr, usize size) {
  if (fd < 0) return -1;
  if (addr == null) return -1;

  handle_t file = avltree_get(current_task->resman->avltree, fd);

  // 检查文件句柄是否合法
  if (file == null) return -1;
  if (file->fd == null) return -1;
  if (file->fd->node == null) return -1;
  if (file->fd->node->type == file_dir) return -1;

  isize write_bytes = vfs_write(file->fd->node, addr, file->offset, size);

  file->offset += write_bytes;

  return write_bytes;
}

int syscall_close(int fd) {
  if (fd < 0) return -1;
  return resman_close(current_task->resman, fd);
}
