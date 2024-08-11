#pragma once
#include <libc-base.h>

#define SYSCALL_EXIT    0
#define SYSCALL_PUTC    1
#define SYSCALL_FORK    2
#define SYSCALL_PRINT   3
#define SYSCALL_GETHEAP 4
#define SYSCALL_HEAPSZ  5
#define SYSCALL_MMAP    6
#define SYSCALL_MUNMAP  7
#define SYSCALL_READ    8
#define SYSCALL_WRITE   9
#define MAX_SYSCALLS    256

#ifndef RING0
dlimport ssize_t syscall(ssize_t id, ...);

dlimport void exit(int status) __attr(noreturn);

dlimport int print(cstr s);

dlimport void *mmap(void *addr, size_t size);

dlimport void munmap(void *addr, size_t size);
#endif
