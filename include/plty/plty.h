#pragma once
#include <font.h>
#include <libc-base.h>

typedef struct plty_char {
  font_char_t ch;
  color_t     fg, bg;
} *plty_char_t;

typedef struct plty {
  plty_char_t text;               // 行缓冲区
  plty_char_t text2;              // 行缓冲区
  u32         ncols;              // 每行的字符数
  u32         nlines;             // 行数
  u16         charw, charh;       // 字符的宽高
  void       *vram;               // 显存
  u32         width;              // 终端的宽度
  u32         height;             // 终端的高度
  plff_t      fonts[4];           // 字体
  color_t     fg, bg;             // 前景色和背景色
  u32         cur_x, cur_y;       // 当前光标的位置
  color_t     cur_fg, cur_bg;     // 当前字符的颜色
  u32         cur_oldx, cur_oldy; // 当前光标的位置
  bool        auto_flush;         // 在换行时自动刷新
  bool        show_cur;           // 在换行时自动刷新
  char       *input_buf;          // 输入缓冲区
  size_t      input_bufsize;      // 输入缓冲区大小
} *plty_t;

plty_t plty_alloc(void *vram, size_t width, size_t height, plff_t font);

void plty_free(plty_t tty);

void plty_addfont(plty_t tty, plff_t font);

void plty_flush(plty_t tty);

void plty_clear(plty_t tty);

void plty_scroll(plty_t tty);

color_t plty_getfg(plty_t tty);

color_t plty_getbg(plty_t tty);

void plty_setfg(plty_t tty, color_t fg);

void plty_setbg(plty_t tty, color_t bg);

color_t plty_getpfg(plty_t tty, i32 x, i32 y);

color_t plty_getpbg(plty_t tty, i32 x, i32 y);

void plty_setpfg(plty_t tty, i32 x, i32 y, color_t fg);

void plty_setpbg(plty_t tty, i32 x, i32 y, color_t bg);

u32 plty_getch(plty_t tty, i32 x, i32 y);

void plty_setch(plty_t tty, i32 x, i32 y, u32 ch);

void plty_getcur(plty_t tty, u32 *x, u32 *y);

u32 plty_getcurx(plty_t tty);

u32 plty_getcury(plty_t tty);

void plty_setcur(plty_t tty, u32 x, u32 y);

void plty_curnext(plty_t tty, u32 i);

void plty_curprev(plty_t tty, u32 i);

void plty_putc(plty_t tty, u32 c);

void plty_pututf8c(plty_t tty, byte c);

void plty_puts(plty_t tty, cstr s);
