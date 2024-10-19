#include <loader.h>

static char buf[1024];

int printf(cstr fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  var len = vsprintf(buf, fmt, ap);
  print(buf);
  va_end(ap);
  return len;
}

int klogf(cstr format, ...) {
  va_list ap;
  va_start(ap, format);
  var len = vsprintf(buf, format, ap);
  print(buf);
  va_end(ap);
  return len;
}

int snprintf(char *str, size_t size, cstr format, ...) {
  va_list args;
  va_start(args, format);
  int len = vsprintf(str, format, args);
  va_end(args);
  return len;
}
