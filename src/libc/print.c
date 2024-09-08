// This code is released under the MIT License

#include <libc.h>

int printf(cstr _rest fmt, ...) {
  static char buf[4096];
  va_list     va;
  va_start(va, fmt);
  int rets = vsprintf(buf, fmt, va);
  va_end(va);
  print(buf);
  return rets;
}

int puts(cstr s) {
  print(s);
  print("\n");
  return 0;
}
