#pragma once
#include <libc.h>

enum fmtalign {
  align_left,
  align_middle,
  align_right,
};

typedef struct fmtarg {
  int align; // 对齐方式

  int n; // 当前已格式化的字符数

  // 背景颜色
  bool setfg : 1, setbg : 1;
  struct {
    byte r, g, b;
  } fg, bg;

  int decimal; // 小数部分的长度

  ssize_t maxlen; // 输出字符串的最大长度
  ssize_t minlen; // 输出字符串的最小长度

  // 格式化时的缓冲区
  char  *buf;
  size_t bufsize;

  // 原数据直接转换成的字符串
  // 包括结尾的 '\0'
  // 不包括用于对齐的空格
  // 指向 buf 中的内容
  char *text;
} fmtarg;

dlimport cstr sprint_foramt(fmtarg *_rest arg, cstr _rest fmt, va_list va);

finline void fmtarg_clear(fmtarg *arg) {
  arg->align   = align_left;
  arg->setfg   = false;
  arg->setfg   = false;
  arg->decimal = 0;
  arg->maxlen  = 0;
  arg->minlen  = 0;
  arg->text    = null;
}
