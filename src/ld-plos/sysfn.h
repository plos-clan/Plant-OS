#pragma once
#include <libc-base.h>
#include <sys.h>

finline void println(cstr text) {
  syscall(SYSCALL_PRINT, text);
  syscall(SYSCALL_PRINT, "\n");
}
