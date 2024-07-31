#pragma once
#include <sys/syscall.h>

void syscall_exit(int status);
void syscall_putchar(int ch);
int  syscall_fork();
