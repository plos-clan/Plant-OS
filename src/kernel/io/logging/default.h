#include <kernel.h>

#ifndef PORT
#  warning "PORT is not defined"
#  define PORT 0
#endif

//* ----------------------------------------------------------------------------------------------------
//; 基本日志记录函数
//* ----------------------------------------------------------------------------------------------------

finline void _putb(int c) {
  waituntil(asm_in8(PORT + 5) & 0x20);
  asm_out8(PORT, c);
}

finline void _puts(cstr s) {
  for (size_t i = 0; s[i] != '\0'; i++) {
    _putb(s[i]);
  }
}

bool debug_enabled = false;

char debug_shell_message[256];

// 输出函数
finline void log_outs(cstr s) {
  if (s == null) return;
  for (size_t i = 0; s[i] != '\0'; i++) {
    _putb(s[i]);
  }
}

void log_update() {
  // Debug is off.
}

void klog_raw(cstr s) {
  with_no_interrupts(log_outs(s));
}

static char klog_buf[4096];

void klog(cstr _rest fmt, ...) {
  with_no_interrupts({
    va_list va;
    va_start(va, fmt);
    vsprintf(klog_buf, fmt, va);
    va_end(va);
    log_outs(klog_buf);
  });
}

static char print_buf[4096];

void kprintf(cstr _rest fmt1, cstr _rest fmt2, ...) {
  va_list va1, va2;
  va_start(va1, fmt2);
  va_copy(va2, va1);
  vsprintf(print_buf, fmt1, va1);
  klog_raw(print_buf);
  vsprintf(print_buf, fmt2, va2);
  print(print_buf);
  va_end(va2);
  va_end(va1);
}

int printf(cstr _rest fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int rets = vsprintf(print_buf, fmt, va);
  va_end(va);
  print(print_buf);
  size_t len = strlen(print_buf);
  while (len > 0 && print_buf[len - 1] == '\n') {
    print_buf[len - 1] = '\0';
    len--;
  }
  klogi("print: %s", print_buf);
  return rets;
}

void puts(cstr s) {
  print(s);
  print("\n");
  klogi("print: %s", s);
}

void kenel_debugger_tick() {
  // Debug is off.
}
