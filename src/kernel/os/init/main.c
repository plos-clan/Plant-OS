#include <define.h>
#include <kernel.h>
#include <type.h>

void write_serial(char a);

int printf(cstr _rest fmt, ...) {
  static char buf[4096];
  va_list     va;
  va_start(va, fmt);
  int rets = vsprintf(buf, fmt, va);
  va_end(va);
  for (int i = 0; i < rets; i++) {
    write_serial(buf[i]);
  }
  return rets;
}

void kernel_main() {
  sysinit();

  printf("字符测试: %c\n", 'A');
  printf("数字测试: %d\n", 114514);
  printf("字符串测试: %s\n", "Hello world!");

  printd("一条测试 debug 消息");
  printi("一条测试 info 消息");
  printw("一条测试 warning 消息");
  printe("一条测试 error 消息");

  // *(u8 *)(0xb8000) = 'K';
  // *(u8 *)(0xb8001) = 0x0c;

  while (true) {}
}
