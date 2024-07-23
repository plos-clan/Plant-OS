#pragma once
#include <libc.h>

// 终端颜色序号
enum {
  tty_black,
  tty_dark_blue,
  tty_dark_green,
  tty_dark_cyan,
  tty_dark_red,
  tty_dark_magenta,
  tty_dark_yellow,
  tty_light_gray,
  tty_dark_gray,
  tty_blue,
  tty_green,
  tty_cyan,
  tty_red,
  tty_magenta,
  tty_yellow,
  tty_white,
};

typedef struct {
  u16   *buf;           //
  size_t width, height; // 终端缓冲区的宽高
  size_t cur_x, cur_y;  // 当前光标的位置
  size_t cur_color;
} plty_fb;

finline void plty_fb_clear(plty_fb *fb) {
  for (size_t i = 0; i < fb->width * fb->height; i++) {
    fb->buf[i] = ' ';
  }
  fb->cur_x     = 0;
  fb->cur_y     = 0;
  fb->cur_color = (tty_light_gray << 8) | (tty_black << 12);
}

finline int plty_fb_getcolor(plty_fb *fb, int x, int y) {
  return fb->buf[y * fb->width + x] >> 8;
}

finline void plty_fb_setcolor(plty_fb *fb, int fg, int bg) {
  fb->cur_color = ((u8)(fg & 15) << 8) | ((u8)(bg & 15) << 12);
}

finline int plty_fb_getfg(plty_fb *fb, int x, int y) {
  return (fb->buf[y * fb->width + x] >> 8) & 15;
}

finline int plty_fb_getbg(plty_fb *fb, int x, int y) {
  return fb->buf[y * fb->width + x] >> 12;
}

finline int plty_fb_getch(plty_fb *fb, int x, int y) {
  return fb->buf[y * fb->width + x] & 0xff;
}

finline void plty_fb_setch(plty_fb *fb, int x, int y, int c) {
  fb->buf[y * fb->width + x] = (u8)c | fb->cur_color;
}

finline void plty_fb_lf(plty_fb *fb) {
  fb->cur_x = 0;
  if (fb->cur_y == fb->height - 1) {
    memcpy(fb->buf, fb->buf + fb->width, 2 * fb->width * (fb->height - 1));
  } else {
    fb->cur_y++;
  }
}

finline void plty_fb_putc(plty_fb *fb, int c) {
  u16 ch = (u8)c | fb->cur_color;
  if (c == '\r') {
    fb->cur_x = 0;
  } else if (c == '\n') {
    plty_fb_lf(fb);
  } else {
    if (fb->cur_x == fb->width) plty_fb_lf(fb);
    if (c == '\t') {
      size_t next_x = (fb->cur_x & ~7) + 8;
      if (next_x > fb->width) next_x = fb->width;
      for (size_t i = fb->cur_x; i < next_x; i++) {
        fb->buf[fb->cur_y * fb->width + i] = ' ' | fb->cur_color;
      }
      fb->cur_x = next_x;
    } else {
      fb->buf[fb->cur_y * fb->width + fb->cur_x] = ch;
      fb->cur_x++;
    }
  }
}

finline void plty_fb_puts(plty_fb *fb, cstr s) {
  for (; *s != '\0'; s++) {
    plty_fb_putc(fb, *s);
  }
}
