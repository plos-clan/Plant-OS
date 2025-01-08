#include <kernel.h>

#ifndef PORT
#  warning "PORT is not defined"
#  define PORT 0
#endif

//* ----------------------------------------------------------------------------------------------------
//; 基本日志记录函数
//* ----------------------------------------------------------------------------------------------------

bool debug_enabled = false;

char debug_shell_message[256];

void log_update() {
  // Debug is off.
}

void klog_raw(cstr s) {
  // Loggin is off.
}

void klog(cstr _rest fmt, ...) {
  // Loggin is off.
}

static char print_buf[4096];

void kprintf(cstr _rest fmt1, cstr _rest fmt2, ...) {
  va_list va;
  va_start(va, fmt2);
  vsprintf(print_buf, fmt2, va);
  print(print_buf);
  va_end(va);
}

int printf(cstr _rest fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int rets = vsprintf(print_buf, fmt, va);
  va_end(va);
  print(print_buf);
  return rets;
}

void puts(cstr s) {
  print(s);
  print("\n");
}

void kenel_debugger_tick() {
  // Debug is off.
}
