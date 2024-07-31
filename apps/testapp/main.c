#include <base.h>
#include <sys/syscall.h>

void print(char *s) {
  while (*s) {
    __syscall(SYSCALL_PUTC, *s++);
  }
}

void _start() {
  if (__syscall(SYSCALL_FORK)) {
    asm volatile("xchg %bx,%bx");
    __syscall(SYSCALL_PUTC, 'a');
  } else {
    __syscall(SYSCALL_PUTC, 'b');
  }
  // syscall(SYSCALL_EXIT, 0);
  for (;;)
    ;
}
