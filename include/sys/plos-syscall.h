#pragma once
#include <libc-base.h>

enum {
  SYSCALL_EXIT    = 0,
  SYSCALL_PUTC    = 1,
  SYSCALL_FORK    = 2,
  SYSCALL_PRINT   = 3,
  SYSCALL_GETHEAP = 4,
  SYSCALL_HEAPSZ  = 5,
  SYSCALL_MMAP    = 6,
  SYSCALL_MUNMAP  = 7,
  SYSCALL_READ    = 8,
  SYSCALL_WRITE   = 9,
  MAX_SYSCALLS    = 256,
};

#if !RING0 && NO_STD
dlimport ssize_t syscall(ssize_t id, ...);

dlimport void exit(int status) __attr(noreturn);

dlimport ssize_t print(cstr s);

dlimport void *mmap(void *addr, size_t size) __THROW;

dlimport int munmap(void *addr, size_t size) __THROW;
#endif
