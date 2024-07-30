#include <libc/asm/syscall-plos.h>

void _start() {
  int     a = __syscall(1);
  ssize_t ebx, ecx, edx, esi, edi;
  asm volatile("mov %%ebx, %0\n\t" : "=r"(ebx)::"memory");
  asm volatile("mov %%ecx, %0\n\t" : "=r"(ecx)::"memory");
  asm volatile("mov %%edx, %0\n\t" : "=r"(edx)::"memory");
  asm volatile("mov %%esi, %0\n\t" : "=r"(esi)::"memory");
  asm volatile("mov %%edi, %0\n\t" : "=r"(edi)::"memory");
  __syscall(1, ebx, ecx, edx, esi, edi);
  __syscall(0, a);
  *(char *)0xb8000 = 'H';
  for (;;)
    ;
}
