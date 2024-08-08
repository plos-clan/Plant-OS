#include <define.h>
#include <kernel.h>
#include <plty.h>

void write_serial(char a);

struct color24 {
  u8 r, g, b;
};

finline int color_diff(struct color24 c1, struct color24 c2) {
  int d1 = c1.r - c2.r;
  int d2 = c1.g - c2.g;
  int d3 = c1.b - c2.b;
  return d1 * d1 + d2 * d2 + d3 * d3;
}

static int color_best_match(struct color24 c) {
  static struct color24 _table[] = {
      {0,   0,   0  },
      {0,   0,   128},
      {0,   128, 0  },
      {0,   128, 128},
      {128, 0,   0  },
      {128, 0,   128},
      {128, 128, 0  },
      {192, 192, 192},
      {128, 128, 128},
      {0,   0,   255},
      {0,   255, 0  },
      {0,   255, 255},
      {255, 0,   0  },
      {255, 0,   255},
      {255, 255, 0  },
      {255, 255, 255},
  };
  int best_id  = 0;
  int diff_min = color_diff(c, _table[0]);
  logw("%02d %-6d", 0, diff_min);
  for (int i = 1; i < lengthof(_table); i++) {
    int diff = color_diff(c, _table[i]);
    logw("%02d %-6d", i, diff);
    if (diff < diff_min) {
      best_id  = i;
      diff_min = diff;
    }
  }
  logw("最佳: %02d %-6d", best_id, diff_min);
  return best_id;
}

#define PLTY_FB_BUFSIZE 64

typedef struct {
  u16           *vbuf;                 //
  byte           buf[PLTY_FB_BUFSIZE]; //
  ssize_t        len;                  //
  struct color24 fg, bg;               //
  size_t         width, height;        // 终端缓冲区的宽高
  size_t         cur_x, cur_y;         // 当前光标的位置
  size_t         cur_color;
} plty_fb;

finline void plty_fb_clear(plty_fb *fb) {
  for (size_t i = 0; i < fb->width * fb->height; i++) {
    fb->vbuf[i] = ' ';
  }
  fb->len       = -1;
  fb->cur_x     = 0;
  fb->cur_y     = 0;
  fb->cur_color = (tty_light_gray << 8) | (tty_black << 12);
}

finline int plty_fb_getcolor(plty_fb *fb, int x, int y) {
  return fb->vbuf[y * fb->width + x] >> 8;
}

finline void plty_fb_setcolor(plty_fb *fb, int fg, int bg) {
  fb->cur_color = ((u8)(fg & 15) << 8) | ((u8)(bg & 15) << 12);
}

finline int plty_fb_getfg(plty_fb *fb, int x, int y) {
  return (fb->vbuf[y * fb->width + x] >> 8) & 15;
}

finline int plty_fb_getbg(plty_fb *fb, int x, int y) {
  return fb->vbuf[y * fb->width + x] >> 12;
}

finline int plty_fb_getch(plty_fb *fb, int x, int y) {
  return fb->vbuf[y * fb->width + x] & 0xff;
}

finline void plty_fb_setch(plty_fb *fb, int x, int y, int c) {
  fb->vbuf[y * fb->width + x] = (u8)c | fb->cur_color;
}

finline void plty_fb_lf(plty_fb *fb) {
  fb->cur_x = 0;
  if (fb->cur_y == fb->height - 1) {
    memcpy(fb->vbuf, fb->vbuf + fb->width, 2 * fb->width * (fb->height - 1));
  } else {
    fb->cur_y++;
  }
}

static void plty_fb_putc_raw(plty_fb *fb, int c) {
  u16 ch = (u8)c | fb->cur_color;

  if (c == '\r') {
    fb->cur_x = 0;
    return;
  }

  if (c == '\n') {
    plty_fb_lf(fb);
    return;
  }

  if (fb->cur_x == fb->width) plty_fb_lf(fb); // 当前行已满就换行

  if (c == '\t') {
    size_t next_x = (fb->cur_x & ~7) + 8;
    if (next_x > fb->width) next_x = fb->width;
    for (size_t i = fb->cur_x; i < next_x; i++) {
      fb->vbuf[fb->cur_y * fb->width + i] = ' ' | fb->cur_color;
    }
    fb->cur_x = next_x;
    return;
  }

  fb->vbuf[fb->cur_y * fb->width + fb->cur_x] = ch;
  fb->cur_x++;
}

finline void plty_fb_puts_raw(plty_fb *fb, cstr s) {
  for (; *s != '\0'; s++) {
    plty_fb_putc_raw(fb, *s);
  }
}

finline void plty_fb_applyrgb(plty_fb *fb) {
  int fg = color_best_match(fb->fg);
  int bg = color_best_match(fb->bg);
  plty_fb_setcolor(fb, fg, bg);
}

static void plty_fb_parse(plty_fb *fb) {
  fb->buf[fb->len] = '\0';
  if (fb->len == PLTY_FB_BUFSIZE - 1) goto err;

  char *s = (char *)fb->buf;

  if (*s == '[') {
    if (fb->len == 1) goto again;
    s++;
    char *_s;
    int   n = strb10toi(s, &_s);
    if (s == _s) goto err;
    s = _s;
    if (*s++ != ';') goto err;
    if (n == 38) { // 设置前景
      int n = strb10toi(s, &s);
      if (n == 2) {}
    }
    if (n == 48) { // 设置背景
      int n = strb10toi(s, &s);
    }
  }

  goto end;
err:
  plty_fb_puts_raw(fb, (cstr)fb->buf);
end:
  fb->len = -1; // 退出转义模式
again:
  return;
}

finline void plty_fb_putc(plty_fb *fb, int c) {
  if (fb->len >= 0) { // 处于转义序列中
    fb->buf[fb->len++] = c;
    plty_fb_parse(fb);
    if (fb->len < 0) { // 转义序列结束
      plty_fb_applyrgb(fb);
    }
    return;
  }

  if (c == '\033') { // 转义序列开始
    fb->len = 0;
    return;
  }

  plty_fb_putc_raw(fb, c);
}

finline void plty_fb_puts(plty_fb *fb, cstr s) {
  for (; *s != '\0'; s++) {
    plty_fb_putc(fb, *s);
  }
}

// ----------------------------------------------------------------------------------------------------
//- main

plty_fb tty;

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
  // tty.vbuf   = (void *)0xb8000;
  // tty.width  = 80;
  // tty.height = 25;
  // plty_fb_clear(&tty);

  format_test();

  // tty.fg = (struct color24){64, 192, 128};
  // tty.bg = (struct color24){0, 0, 0};
  // plty_fb_applyrgb(&tty);
  // plty_fb_puts(&tty, "123456\n");
  // plty_fb_puts(&tty, "test-end\n");

  // logd("一条测试 debug 消息");
  // logi("一条测试 info 消息");
  // logw("一条测试 warning 消息");
  // loge("一条测试 error 消息");
  // logf("一条测试 fatal 消息");

  sysinit();
  while (true) {}
}
