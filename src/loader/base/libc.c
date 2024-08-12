#include <loader.h>

void printk(char *s, ...) {}

static char *insert_str(char *buf, const char *str) {
  char *p = buf;

  while (*str) {
    *p++ = *str++;
  }

  return p;
}

int printf(cstr fmt, ...) {
  int     len;
  va_list ap;
  va_start(ap, fmt);
  char buf[1024];
  len = vsprintf(buf, fmt, ap);
  print(buf);
  va_end(ap);
  return len;
}

int klogf(const char *format, ...) {
  int     len;
  va_list ap;
  va_start(ap, format);
  char buf[1024];
  len = vsprintf(buf, format, ap);
  print(buf);
  va_end(ap);
  return len;
}

int _Znaj(u32 size) {
  printk("_Znaj:%d\n", size);
  return (int)malloc(size);
}

void _ZdaPv(void *ptr) {
  printk("_ZdaPv:%08x\n", ptr);
  free(ptr);
}

void _ZdlPvj(void *ptr, u32 size) {
  printk("_ZdlPvj %08x %d\n", ptr, size);
  free(ptr);
}

void *_Znwj(u32 size) {
  printk("_Znwj:%d\n", size);
  return malloc(size);
}

int snprintf(char *str, size_t size, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int len = vsprintf(str, format, args);
  va_end(args);
  return len;
}

void *calloc(u32 num, u32 size) {
  return malloc(num * size);
}
