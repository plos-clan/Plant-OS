#include <kernel.h>

// 一些用于日志记录和打印的函数。

void write_serial(char a);

void klog_raw(cstr s) {
  for (size_t i = 0; s[i] != '\0'; i++) {
    write_serial(s[i]);
  }
}

void klog(cstr _rest fmt, ...) {
  static char buf[4096];
  va_list     va;
  va_start(va, fmt);
  vsprintf(buf, fmt, va);
  va_end(va);
  klog_raw(buf);
}

static char print_buf[4096];

void log_printf(cstr _rest fmt1, cstr _rest fmt2, ...) {
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
}
