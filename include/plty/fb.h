#pragma once
#include <libc.h>

typedef struct {
  char **line_buffer;
  size_t line_maxlen;   // line_buffer 中一行的最大长度 (字节)
  size_t max_lines;     // line_buffer 的行数
  size_t width, height; // 终端缓冲区的宽高
  size_t cur_x, cur_y;  // 当前光标的位置
} plty_fb;

void plty_fb_clear(plty_fb *fb) {
  if (fb == null) return;
}
