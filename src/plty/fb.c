#include "color-match.h"
#include <plty.h>

// ----------------------------------------------------------------------------------------------------

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
  int fg = color_best_match16(fb->fg);
  int bg = color_best_match16(fb->bg);
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
