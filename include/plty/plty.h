#pragma once
#include <font.h>
#include <libc-base.h>

/**
 *\brief Plant-OS 终端实现
 *
 */

// TODO 支持动态改变终端大小
// TODO 封装以便移植
// TODO 支持转义序列
// TODO 支持输入

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
  void       *backbuf;            // 后台缓冲区
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
  void (*flipbuf)();              //
} *plty_t;

/**
 *\brief 创建一个终端对象
 *
 *\param vram     显示缓冲区地址
 *\param width    缓冲区宽度（像素）
 *\param height   缓冲区高度（像素）
 *\param font     字体
 *\return 终端对象
 */
plty_t plty_alloc(void *vram, size_t width, size_t height, plff_t font);

/**
 *\brief 释放给定的TTY资源
 *
 *\param tty 要释放的TTY对象
 */
void plty_free(plty_t tty);

/**
 *\brief 设置TTY的后备缓冲区
 *
 *\param tty TTY对象
 *\param vram 后备缓冲区指针
 *\param flipbuf 缓冲区翻转函数指针
 */
void plty_setbackbuffer(plty_t tty, void *vram, void (*flipbuf)());

/**
 *\brief 向TTY添加字体
 *
 *\param tty TTY对象
 *\param font 字体对象
 */
void plty_addfont(plty_t tty, plff_t font);

/**
 *\brief 刷新TTY显示
 *
 *\param tty TTY对象
 */
void plty_flush(plty_t tty);

/**
 *\brief 清除TTY显示
 *
 *\param tty TTY对象
 */
void plty_clear(plty_t tty);

/**
 *\brief 滚动TTY显示
 *
 *\param tty TTY对象
 */
void plty_scroll(plty_t tty);

/**
 *\brief 获取TTY的前景色
 *
 *\param tty TTY对象
 *\return 前景色
 */
color_t plty_getfg(plty_t tty);

/**
 *\brief 获取TTY的背景色
 *
 *\param tty TTY对象
 *\return 背景色
 */
color_t plty_getbg(plty_t tty);

/**
 *\brief 设置TTY的前景色
 *
 *\param tty TTY对象
 *\param fg 前景色
 */
void plty_setfg(plty_t tty, color_t fg);

/**
 *\brief 设置TTY的背景色
 *
 *\param tty TTY对象
 *\param bg 背景色
 */
void plty_setbg(plty_t tty, color_t bg);

/**
 *\brief 获取TTY指定位置的前景色
 *
 *\param tty TTY对象
 *\param x X坐标
 *\param y Y坐标
 *\return 指定位置的前景色
 */
color_t plty_getpfg(plty_t tty, i32 x, i32 y);

/**
 *\brief 获取TTY指定位置的背景色
 *
 *\param tty TTY对象
 *\param x X坐标
 *\param y Y坐标
 *\return 指定位置的背景色
 */
color_t plty_getpbg(plty_t tty, i32 x, i32 y);

/**
 *\brief 设置TTY指定位置的前景色
 *
 *\param tty TTY对象
 *\param x X坐标
 *\param y Y坐标
 *\param fg 前景色
 */
void plty_setpfg(plty_t tty, i32 x, i32 y, color_t fg);

/**
 *\brief 设置TTY指定位置的背景色
 *
 *\param tty TTY对象
 *\param x X坐标
 *\param y Y坐标
 *\param bg 背景色
 */
void plty_setpbg(plty_t tty, i32 x, i32 y, color_t bg);

/**
 *\brief 获取TTY指定位置的字符
 *
 *\param tty TTY对象
 *\param x X坐标
 *\param y Y坐标
 *\return 指定位置的字符
 */
u32 plty_getch(plty_t tty, i32 x, i32 y);

/**
 *\brief 设置TTY指定位置的字符
 *
 *\param tty TTY对象
 *\param x X坐标
 *\param y Y坐标
 *\param ch 字符
 */
void plty_setch(plty_t tty, i32 x, i32 y, u32 ch);

/**
 *\brief 获取TTY光标位置
 *
 *\param tty TTY对象
 *\param x 存储光标X坐标的指针
 *\param y 存储光标Y坐标的指针
 */
void plty_getcur(plty_t tty, u32 *x, u32 *y);

/**
 *\brief 获取TTY光标的X坐标
 *
 *\param tty TTY对象
 *\return 光标的X坐标
 */
u32 plty_getcurx(plty_t tty);

/**
 *\brief 获取TTY光标的Y坐标
 *
 *\param tty TTY对象
 *\return 光标的Y坐标
 */
u32 plty_getcury(plty_t tty);

/**
 *\brief 设置TTY光标位置
 *
 *\param tty TTY对象
 *\param x 光标的X坐标
 *\param y 光标的Y坐标
 */
void plty_setcur(plty_t tty, u32 x, u32 y);

/**
 *\brief 将TTY光标向前移动指定的步数
 *
 *\param tty TTY对象
 *\param i 步数
 */
void plty_curnext(plty_t tty, u32 i);

/**
 *\brief 将TTY光标向后移动指定的步数
 *
 *\param tty TTY对象
 *\param i 步数
 */
void plty_curprev(plty_t tty, u32 i);

/**
 *\brief 在TTY上输出一个字符
 *
 *\param tty TTY对象
 *\param c 字符
 */
void plty_putc(plty_t tty, u32 c);

/**
 *\brief 在TTY上输出一个UTF-8字符
 *
 *\param tty TTY对象
 *\param c UTF-8字符
 */
void plty_pututf8c(plty_t tty, byte c);

/**
 *\brief 在TTY上输出一个字符串
 *
 *\param tty TTY对象
 *\param s 字符串
 */
void plty_puts(plty_t tty, cstr s);
