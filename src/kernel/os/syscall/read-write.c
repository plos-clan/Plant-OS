#include <kernel.h>

// enum {
//   read,
//   write,
//   seek,
//   open,
//   close,
// };

ssize_t syscall_read(int fd, void *addr, size_t size) {
  if (fd < 0) return -1;
  if (addr == null) return -1;
  return 0;
}

ssize_t syscall_write(int fd, const void *addr, size_t size) {
  if (fd < 0) return -1;
  if (addr == null) return -1;
  return 0;
}
