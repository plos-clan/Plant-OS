#pragma once
#include <sys/plos-syscall.h>

void    syscall_exit(int status);
int     syscall_fork();
void   *syscall_getheap();
u32     syscall_heapsize();
void   *syscall_mmap(void *start, u32 length);
void    syscall_munmap(void *start, u32 length);
ssize_t syscall_read(int fd, void *addr, size_t size);
ssize_t syscall_write(int fd, const void *addr, size_t size);
