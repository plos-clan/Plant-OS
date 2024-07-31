#pragma once
#include <libc-base.h>

#define SYSCALL_EXIT 0
#define SYSCALL_PUTC 1
#define SYSCALL_FORK 2
#define MAX_SYSCALLS 256

#ifndef KERNEL
dlimport ssize_t syscall(ssize_t id, ...);
#endif
