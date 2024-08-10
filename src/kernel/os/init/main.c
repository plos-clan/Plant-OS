#include <define.h>
#include <kernel.h>
#include <plty.h>

void write_serial(char a);

void format_test() {
  klogi("开始格式化测试");

  klog("字符测试: %c\n", 'A');
  klog("数字测试: %d\n", 114514);
  klog("字符串测试: %s\n", "Hello world!");

  klog("对齐测试: %10d\n", 1234);
  klog("对齐测试: %-10d\n", 1234);

  klog("零填充对齐测试: %010d\n", 1234);

  klog("浮点测试: %lf\n", 123.45);
  klog("浮点测试: %lf\n", 1145141919810.);

  klogi("格式化测试结束");
}

void kernel_main() {
  // tty.vbuf   = (void *)0xb8000;
  // tty.width  = 80;
  // tty.height = 25;
  // plty_fb_clear(&tty);

  format_test();

  sysinit();
  while (true) {}
}
