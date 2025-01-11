#include <kernel.h>

// enum {
//   read,
//   write,
//   seek,
//   open,
//   close,
// };

isize syscall_read(int fd, void *addr, usize size) {
  if (fd < 0) return -1;
  if (addr == null) return -1;
  return 0;
}

isize syscall_write(int fd, const void *addr, usize size) {
  if (fd < 0) return -1;
  if (addr == null) return -1;
  return 0;
}
