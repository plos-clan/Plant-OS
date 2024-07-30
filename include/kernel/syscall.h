#pragma once
#include <define.h>
#include <type.h>

#define SYSCALL_EXIT 0
#define MAX_SYSCALLS 256

void syscall_exit(int status);
