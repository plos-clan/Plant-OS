#pragma once
#include <sys/plos-syscall.h>

#ifdef __x86_64__
typedef isize (*syscall_t)(isize, isize, isize, isize, isize, isize);
#else
typedef isize (*syscall_t)(isize, isize, isize, isize, isize);
#endif

void    syscall_exit(int status);
int     syscall_fork();
void   *syscall_mmap(void *start, u32 length);
void    syscall_munmap(void *start, u32 length);
ssize_t syscall_read(int fd, void *addr, size_t size);
ssize_t syscall_write(int fd, const void *addr, size_t size);
int     syscall_open(const char *path, int flags, int mode);
int     syscall_close(int fd);