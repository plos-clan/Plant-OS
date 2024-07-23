#pragma once
#include <define.h>
#include <kernel/screen.h>
#include <type.h>
struct tty {
  int   is_using;                                     // 使用标志
  void *vram;                                         // 显存（也可以当做图层）
  int   x, y;                                         // 目前的 x y 坐标
  int   xsize, ysize;                                 // x 坐标大小 y 坐标大小
  int   Raw_y;                                        // 换行次数
  int   cur_moving;                                   // 光标需要移动吗
  u8    color;                                        // 颜色
  void (*putchar)(struct tty *res, int c);            // putchar函数
  void (*MoveCursor)(struct tty *res, int x, int y);  // MoveCursor函数
  void (*clear)(struct tty *res);                     // clear函数
  void (*screen_ne)(struct tty *res);                 // screen_ne函数
  void (*gotoxy)(struct tty *res, int x, int y);      // gotoxy函数
  void (*print)(struct tty *res, const char *string); // print函数
  void (*Draw_Box)(struct tty *res, int x, int y, int x1, int y1,
                   u8 color); // Draw_Box函数
  int (*fifo_status)(struct tty *res);
  int (*fifo_get)(struct tty *res);
  u32 reserved[4]; // 保留项

  //////////////实现VT100需要的//////////////////

  int          vt100;       // 是否检测到标志
  char         buffer[81];  // 缓冲区
  int          buf_p;       // 缓冲区指针
  int          done;        // 这个东西读取完毕没有？
  vt100_mode_t mode;        // 控制模式
  int          color_saved; // 保存的颜色
};
struct tty *tty_alloc(void *vram, int xsize, int ysize, void (*putchar)(struct tty *res, int c),
                      void (*MoveCursor)(struct tty *res, int x, int y),
                      void (*clear)(struct tty *res), void (*screen_ne)(struct tty *res),
                      void (*Draw_Box)(struct tty *res, int x, int y, int x1, int y1,
                                       unsigned char color),
                      int (*fifo_status)(struct tty *res), int (*fifo_get)(struct tty *res));
void        init_tty();
// textmode
void        MoveCursor_TextMode(struct tty *res, int x, int y);
void        putchar_TextMode(struct tty *res, int c);
void        screen_ne_TextMode(struct tty *res);
void        clear_TextMode(struct tty *res);
void        Draw_Box_TextMode(struct tty *res, int x, int y, int x1, int y1, u8 color);
void        AddShell_TextMode();
void        SwitchShell_TextMode(int i);
bool        now_tty_TextMode(struct tty *res);
