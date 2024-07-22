#include <libc.h>

#define vsprintf_bufsize 1024
static char vsprintf_buf[vsprintf_bufsize];

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

finline void fmtarg_clear(fmtarg *arg) {
  arg->align   = align_left;
  arg->setfg   = false;
  arg->setfg   = false;
  arg->decimal = 0;
  arg->maxlen  = 0;
  arg->minlen  = 0;
  arg->text    = null;
}

static cstr sprint_foramt(fmtarg *arg, cstr fmt, va_list va) {
  fmtarg_clear(arg);
  fmt++;

  if (*fmt == '%') {
    arg->text    = arg->buf;
    arg->text[0] = '%';
    arg->text[1] = '\0';
    return fmt + 1;
  }

  char *_fmt;
  arg->maxlen = strtoi(fmt, &_fmt);
  if (fmt == _fmt && *_fmt == '*') {
    arg->maxlen = va_arg(va, int);
    _fmt++;
  }
  fmt = _fmt;
  if (arg->maxlen < 0) {
    arg->maxlen = -arg->maxlen;
    arg->align  = 1;
  }

  if (*fmt == '.') {
    fmt++;
    arg->decimal = strtoi(fmt, &_fmt);
    if (fmt == _fmt && *_fmt == '*') {
      arg->decimal = va_arg(va, int);
      _fmt++;
    }
    fmt = _fmt;
  }

  // char 1
  // short 2
  // int 3
  // long 4
  // long long 5
  int _size = 3;

  while (1) {
    switch (*fmt++) {

    case 'h':
      if (_size <= 0 || _size > 3) return null; // 解析失败
      _size--;
      break;

    case 'l':
      if (_size < 3 || _size >= 6) return null; // 解析失败
      _size++;
      break;

    case 'c':
    case 'C':
      if (_size != 3) return null; // 解析失败
      *arg->text = va_arg(va, int);
      goto end;

#define __tostr_arg(type)     arg->buf, arg->bufsize, va_arg(va, type)
#define __tostr_arg2(t, type) arg->buf, arg->bufsize, (t)va_arg(va, type)

    case 'b':
    case 'B':
      switch (_size) {
      case 1: arg->text = hhtostrb2(__tostr_arg2(char, int)); break;
      case 2: arg->text = htostrb2(__tostr_arg2(short, int)); break;
      case 3: arg->text = itostrb2(__tostr_arg(int)); break;
      case 4: arg->text = ltostrb2(__tostr_arg(long int)); break;
      case 5: arg->text = lltostrb2(__tostr_arg(long long int)); break;
      }
      goto end;

    case 'o':
    case 'O':
      switch (_size) {
      case 1: arg->text = hhtostrb8(__tostr_arg2(char, int)); break;
      case 2: arg->text = htostrb8(__tostr_arg2(short, int)); break;
      case 3: arg->text = itostrb8(__tostr_arg(int)); break;
      case 4: arg->text = ltostrb8(__tostr_arg(long int)); break;
      case 5: arg->text = lltostrb8(__tostr_arg(long long int)); break;
      }
      goto end;

    case 'i':
    case 'I':
    case 'd':
    case 'D':
      switch (_size) {
      case 1: arg->text = hhtostrb10(__tostr_arg2(char, int)); break;
      case 2: arg->text = htostrb10(__tostr_arg2(short, int)); break;
      case 3: arg->text = itostrb10(__tostr_arg(int)); break;
      case 4: arg->text = ltostrb10(__tostr_arg(long int)); break;
      case 5: arg->text = lltostrb10(__tostr_arg(long long int)); break;
      }
      goto end;

    case 'u':
    case 'U':
      switch (_size) {
      case 1: arg->text = uhhtostrb10(__tostr_arg2(unsigned char, int)); break;
      case 2: arg->text = uhtostrb10(__tostr_arg2(unsigned short, int)); break;
      case 3: arg->text = uitostrb10(__tostr_arg(unsigned int)); break;
      case 4: arg->text = ultostrb10(__tostr_arg(unsigned long int)); break;
      case 5: arg->text = ulltostrb10(__tostr_arg(unsigned long long int)); break;
      }
      goto end;

    case 'x':
      switch (_size) {
      case 1: arg->text = hhtostrb16(__tostr_arg2(char, int)); break;
      case 2: arg->text = htostrb16(__tostr_arg2(short, int)); break;
      case 3: arg->text = itostrb16(__tostr_arg(int)); break;
      case 4: arg->text = ltostrb16(__tostr_arg(long int)); break;
      case 5: arg->text = lltostrb16(__tostr_arg(long long int)); break;
      }
      goto end;
    case 'X':
      switch (_size) {
      case 1: arg->text = hhtostrB16(__tostr_arg2(char, int)); break;
      case 2: arg->text = htostrB16(__tostr_arg2(short, int)); break;
      case 3: arg->text = itostrB16(__tostr_arg(int)); break;
      case 4: arg->text = ltostrB16(__tostr_arg(long int)); break;
      case 5: arg->text = lltostrB16(__tostr_arg(long long int)); break;
      }
      goto end;

    case 'n':
    case 'N':
      if (_size != 3) return null; // 解析失败
      *va_arg(va, int *) = arg->n;
      goto end;

    case 's': // 字符串
    case 'S':
      if (_size != 3) return null; // 解析失败
      arg->text = va_arg(va, void *);
      if (!arg->text) arg->text = arg->buf;
      goto end;

    default: return null; // 解析失败
    }
  }

end:
  return fmt;
}

static cstr sformat_format(fmtarg *arg, cstr fmt, va_list va) {
  return null;
}

static char *vsprintf_align(char *s, fmtarg *arg) {
  if (!arg->text) return s;
  ssize_t arg_len = strlen(arg->text);

  if (arg->maxlen > 0 && arg_len > arg->maxlen) { // 放不下就进行截断 假如 maxlen 为 25
    if (arg->maxlen <= 3) {                       // This is a long long long long line.
      for (size_t i = 0; i < arg->maxlen; i++)    // This is a long long lo...
        *s++ = '.';
    } else {
      for (size_t i = 0; i < arg->maxlen - 3; i++)
        *s++ = arg->text[i];
      for (size_t i = 0; i < 3; i++)
        *s++ = '.';
    }
    return s;
  }

  ssize_t blank      = arg->minlen - arg_len;
  size_t  blank_left = 0, blank_right = 0;

  if (blank > 0) {
    if (arg->align == align_left) {
      blank_right = blank;
    } else if (arg->align == align_middle) {
      blank_left  = blank / 2;
      blank_right = blank - blank_left;
    } else if (arg->align == align_right) {
      blank_left = blank;
    } else {
      blank_right = blank; // 异常值当作左对齐
    }
  }

  for (size_t i = 0; i < blank_left; i++)
    *s++ = ' ';
  while (*arg->text++ != '\0')
    *s++ = *arg->text++;
  for (size_t i = 0; i < blank_right; i++)
    *s++ = ' ';

  return s;
}

static bool vsprintf_tryfmt(char *_rest *_s, char *sb, cstr _rest *fmt, fmtarg *arg, va_list va) {
  if (**fmt != '%') return false;
  arg->n    = *_s - sb;
  cstr _fmt = sprint_foramt(arg, *fmt, va);
  if (_fmt == null) return false;
  *fmt = _fmt;
  *_s  = vsprintf_align(*_s, arg);
  return true;
}

dlexport int vsprintf(char *_rest s, cstr _rest fmt, va_list va) {
  if (s == null || fmt == null) return 0;
  char *s_begin = s;

  static fmtarg arg = {.buf = vsprintf_buf, .bufsize = vsprintf_bufsize};

  while (*fmt != '\0') {
    if (vsprintf_tryfmt(&s, s_begin, &fmt, &arg, va)) continue;
    *s++ = *fmt++;
  }

  return s - s_begin;
}

dlexport int sprintf(char *_rest s, cstr _rest fmt, ...) {
  if (fmt == null) return 0;
  va_list va;
  va_start(va, fmt);
  int rets = vsprintf(s, fmt, va);
  va_end(va);
  return rets;
}

dlexport ssize_t vsformat(char *_rest s, cstr _rest fmt, va_list va) {
  return 0;
}

dlexport ssize_t sformat(char *_rest s, cstr _rest fmt, ...) {
  if (s == null || fmt == null) return 0;
  va_list va;
  va_start(va, fmt);
  ssize_t rets = vsformat(s, fmt, va);
  va_end(va);
}

int vsnprintf(char *_rest s, size_t maxlen, const char *_rest fmt, va_list va) {
  return 0;
}
