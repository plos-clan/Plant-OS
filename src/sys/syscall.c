#include <sys.h>

dlexport ssize_t syscall(ssize_t id, ...) {
  va_list va;
  va_start(va, id);
  ssize_t a1 = va_arg(va, ssize_t);
  ssize_t a2 = va_arg(va, ssize_t);
  ssize_t a3 = va_arg(va, ssize_t);
  ssize_t a4 = va_arg(va, ssize_t);
  ssize_t a5 = va_arg(va, ssize_t);
  va_end(va);
  return __syscall(id, a1, a2, a3, a4, a5);
}
