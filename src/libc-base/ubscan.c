#include <define.h>
#undef dlimport
#define dlimport static
#include <libc-base.h>

#define USE_SPRINTF 1
#undef dlexport
#define dlexport static
#include "stdio/sprint.c"

#define PORT 0x3f8

finline __nnull(1) void log_outs(cstr s) {
  for (size_t i = 0; s[i] != '\0'; i++) {
    waituntil(asm_in8(PORT + 5) & 0x20);
    asm_out8(PORT, s[i]);
  }
}

static void klog_raw(cstr s) {
  var flag = asm_get_flags();
  asm_cli;
  log_outs(s);
  asm_set_flags(flag);
}

static void klog(cstr _rest fmt, ...) {
  var flag = asm_get_flags();
  asm_cli;
  static char buf[4096];
  va_list     va;
  va_start(va, fmt);
  vsprintf(buf, fmt, va);
  va_end(va);
  log_outs(buf);
  asm_set_flags(flag);
}

#define _log_func_ klog
#include <ubscan.h>
