

#include "../stdlib/xtoa.h"
#include "define.h"

struct print_fmtarg {
  // 文本对齐方式
  // 0: 靠右
  // 1: 靠左
  // 2: 居中
  int align;

  int n;

  // 背景颜色
  bool setfg : 1, setbg : 1;
  struct {
    byte r, g, b;
  } fg, bg;

  // 小数部分的长度
  int decimal;

  // 输出字符串的最大长度
  int maxlen;
  // 输出字符串的最小长度
  int minlen;

  // 格式化时的缓冲区
  char  *buf;
  size_t bufsize;

  // 原数据直接转换成的字符串
  // 不包括用于对齐的空格
  // 指向 buf 中的内容
  char  *text;
  // 字符串的长度
  size_t len;
};

// {}
// {}

dlimport cstr sprint_foramt(struct print_fmtarg *_rest arg, cstr _rest fmt, va_list va);

dlimport int vsprintf(char *_rest s, cstr _rest fmt, va_list va);

dlimport int sprintf(char *_rest s, cstr _rest fmt, ...);

dlimport int vsnprintf(char *_rest s, size_t maxlen, cstr _rest fmt, va_list va);
