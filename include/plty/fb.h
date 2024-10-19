#pragma once
#include <libc-base.h>

extern const color_t color_table_16[16];

enum { // 终端颜色序号
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

#define PLTY_FB_BUFSIZE 64

typedef struct plty_fb {
  u16    *vbuf;                 //
  byte    buf[PLTY_FB_BUFSIZE]; //
  ssize_t len;                  //
  color_t fg, bg;               // 整个终端的前景和背景
  size_t  width, height;        // 终端缓冲区的宽高
  size_t  cur_x, cur_y;         // 当前光标的位置
  size_t  cur_color;            // 当前字符的颜色
} plty_fb;

void plty_fb_clear(plty_fb *fb);

int plty_fb_getcolor(plty_fb *fb, int x, int y);

void plty_fb_setcolor(plty_fb *fb, int fg, int bg);

int plty_fb_getfg(plty_fb *fb, int x, int y);

int plty_fb_getbg(plty_fb *fb, int x, int y);

int plty_fb_getch(plty_fb *fb, int x, int y);

void plty_fb_setch(plty_fb *fb, int x, int y, int c);

void plty_fb_lf(plty_fb *fb);

void plty_fb_putc_raw(plty_fb *fb, int c);

void plty_fb_puts_raw(plty_fb *fb, cstr s);

void plty_fb_putc(plty_fb *fb, int c);

void plty_fb_puts(plty_fb *fb, cstr s);
