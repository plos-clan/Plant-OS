#include <define.h>
#include <kernel.h>
#include <plty.h>
#include <type.h>

void write_serial(char a);

plty_fb tty;

void logk_raw(cstr s) {
  // static spin_t spin = false;
  // spin_lock(spin);
  for (size_t i = 0; s[i] != '\0'; i++) {
    write_serial(s[i]);
  }
  // spin_unlock(spin);
  // task_next();
}

int logk(cstr _rest fmt, ...) {
  static char buf[4096];
  va_list     va;
  va_start(va, fmt);
  int rets = vsprintf(buf, fmt, va);
  va_end(va);
  logk_raw(buf);
  return rets;
}

void format_test() {
  logi("开始格式化测试");

  logk("字符测试: %c\n", 'A');
  logk("数字测试: %d\n", 114514);
  logk("字符串测试: %s\n", "Hello world!");

  logk("对齐测试: %10d\n", 1234);
  logk("对齐测试: %-10d\n", 1234);

  logk("零填充对齐测试: %010d\n", 1234);

  logk("浮点测试: %lf\n", 123.45);
  logk("浮点测试: %lf\n", 1145141919810.);

  logi("格式化测试结束");
}

void kernel_main() {
  tty.buf    = (void *)0xb8000;
  tty.width  = 80;
  tty.height = 25;
  plty_fb_clear(&tty);

  format_test();

  plty_fb_puts(&tty, "123456\n");

  logd("一条测试 debug 消息");
  logi("一条测试 info 消息");
  logw("一条测试 warning 消息");
  loge("一条测试 error 消息");
  logf("一条测试 fatal 消息");

  sysinit();
  while (true) {}
}
