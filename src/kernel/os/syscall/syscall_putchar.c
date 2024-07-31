#include <kernel.h>

void syscall_putchar(int ch) {
  putchar(ch);
}