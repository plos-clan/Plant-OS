// This code is released under the MIT License

#include <libc-base.h>

#if NO_STD && USE_SPRINTF

#  define vsprintf_bufsize 1024
static char vsprintf_buf[vsprintf_bufsize];

enum fmtalign {
  align_left,
  align_middle,
  align_right,
};

typedef struct fmtarg {
  int n; // 当前已格式化的字符数

  int  align;         // 对齐方式
  bool fill_zero : 1; // 是否用 0 填充

  bool print_ptr : 1;

  // 背景颜色s
  bool setfg : 1, setbg : 1;
  struct {
    byte r, g, b;
  } fg, bg;

  int decimal; // 小数部分的长度

  ssize_t padding; // 输出字符串的对齐长度
  ssize_t maxlen;  // 输出字符串的最大长度
  ssize_t minlen;  // 输出字符串的最小长度

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
  arg->align     = align_right;
  arg->fill_zero = false;
  arg->print_ptr = false;
  arg->setfg     = false;
  arg->setfg     = false;
  arg->decimal   = 0;
  arg->padding   = 0;
  arg->maxlen    = 0;
  arg->minlen    = 0;
  arg->text      = null;
}

static bool sprint_foramt(fmtarg *arg, cstr _rest *_fmt, va_list *_va) {
  fmtarg_clear(arg);
  cstr fmt = *_fmt + 1;

  // va_list va = *_va;
  va_list va;
  va_copy(va, *_va); // WTF

  if (*fmt == '%') {
    arg->text    = arg->buf;
    arg->text[0] = '%';
    arg->text[1] = '\0';
    *_fmt        = fmt + 1;
    return true;
  }

  if (*fmt == '0') {
    arg->fill_zero = true;
    fmt++;
  }

  if (*fmt == '!') { // 解析自定义 padding 语法，如 !8d !-8d
    fmt++;
    char *f;
    arg->padding = strb10toi(fmt, &f);
    if (fmt == f && *f == '*') {
      arg->padding = va_arg(va, int);
      f++;
    }
    if (arg->padding < 0) {
      arg->padding = -arg->padding;
      arg->align   = align_left;
    }
    fmt = f;
  } else { // 解析标准的写法
    char *f;
    arg->minlen = strb10toi(fmt, &f); // 解析 %15d 这样的写法
    if (fmt == f && *f == '*') {      // 解析 %*d 这样的写法
      arg->minlen = va_arg(va, int);
      f++;
    }
    if (arg->minlen < 0) { // 小于 0 的靠右对齐
      arg->minlen = -arg->minlen;
      arg->align  = align_left;
    }
    fmt = f;
  }

  if (arg->fill_zero && arg->align != align_right) goto err;

  if (*fmt == '.') { // 解析小数
    fmt++;
    char *f;
    arg->decimal = strb10toi(fmt, &f);
    if (fmt == f && *f == '*') {
      arg->decimal = va_arg(va, int);
      f++;
    }
    fmt = f;
  }

  // 1 | char      | error
  // 2 | short     | error
  // 3 | int       | float
  // 4 | long      | double
  // 5 | long long | long double
  int _size = 3;

next_char:
  switch (*fmt++) {

  case 'h':
    if (_size <= 0 || _size > 3) goto err;
    _size--;
    goto next_char;

  case 'l':
    if (_size < 3 || _size >= 6) goto err;
    _size++;
    goto next_char;

  case 'c':
  case 'C':
    if (_size != 3) goto err;
    arg->text      = arg->buf;
    arg->text[0]   = va_arg(va, int);
    arg->text[1]   = '\0';
    arg->fill_zero = false; // 字符不应该使用 0 填充
    goto end;

#  define tostr_arg(type)     arg->buf, arg->bufsize, va_arg(va, type)
#  define tostr_arg2(t, type) arg->buf, arg->bufsize, (t)va_arg(va, type)

#  define format_in_bn(_n_, _u_)                                                                   \
    switch (_size) {                                                                               \
    case 1: arg->text = _u_##hhtostr##_n_(tostr_arg2(_u_##char, _u_##int)); break;                 \
    case 2: arg->text = _u_##htostr##_n_(tostr_arg2(_u_##short, _u_##int)); break;                 \
    case 3: arg->text = _u_##itostr##_n_(tostr_arg(_u_##int)); break;                              \
    case 4: arg->text = _u_##ltostr##_n_(tostr_arg(_u_##long)); break;                             \
    case 5: arg->text = _u_##lltostr##_n_(tostr_arg(_u_##llong)); break;                           \
    }                                                                                              \
    goto end;

  case 'b':
  case 'B': format_in_bn(b2, u);

  case 'o':
  case 'O': format_in_bn(b8, u);

  case 'i':
  case 'I':
  case 'd':
  case 'D': format_in_bn(b10, );

  case 'u':
  case 'U': format_in_bn(b10, u);

  case 'x': format_in_bn(b16, u);
  case 'X': format_in_bn(B16, u);

#  undef format_in_bn

  case 'p':
    if (_size != 3) goto err;
    arg->print_ptr = true;
#  ifdef __x86_64__
    arg->text = u64tostrb16(tostr_arg(u64));
#  else
    arg->text = u32tostrb16(tostr_arg(u32));
#  endif
    goto end;
  case 'P':
    if (_size != 3) goto err;
    arg->print_ptr = true;
#  ifdef __x86_64__
    arg->text = u64tostrB16(tostr_arg(u64));
#  else
    arg->text = u32tostrB16(tostr_arg(u32));
#  endif
    goto end;

  case 'f':
  case 'F':
    switch (_size) {
    case 1: goto err;
    case 2: goto err;
    case 3: arg->text = fftostr(tostr_arg2(float, double)); break;
    case 4: arg->text = ftostr(tostr_arg(double)); break;
    case 5: arg->text = fltostr(tostr_arg(long double)); break;
    }
    goto end;

#  undef tostr_arg
#  undef tostr_arg2

  case 'n':
  case 'N':
    if (_size != 3) goto err;
    *va_arg(va, int *) = arg->n;
    goto end;

  case 's': // 字符串
  case 'S':
    if (_size != 3) goto err;
    arg->text = va_arg(va, char *);
    if (!arg->text) {
      arg->text      = arg->buf;
      arg->text[0]   = '\0';
      arg->fill_zero = false; // 字符串不应该使用 0 填充
    }
    goto end;

  default: goto err;
  }

end:
  *_fmt = fmt;
  // *_va = va;
  va_end(*_va);
  va_copy(*_va, va); // WTF
  return true;

err:
  return false;
}

static bool sformat_format(fmtarg *arg, cstr *_fmt, va_list *_va) {
  return false;
}

static char *vsprintf_align(char *s, fmtarg *arg) {
  if (!arg->text) return s; // 无输出
  ssize_t _arg_len = strlen(arg->text);
  ssize_t arg_len  = arg->print_ptr ? 2 + 2 * sizeof(void *) : _arg_len;

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

  if (arg->padding) arg->minlen = (arg_len + arg->padding - 1) / arg->padding * arg->padding;

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

  if (blank_left > 0 && arg->fill_zero && *arg->text == '-') {
    *s++ = '-';
    arg->text++;
  }
  for (size_t i = 0; i < blank_left; i++)
    *s++ = arg->fill_zero ? '0' : ' ';
  if (arg->print_ptr) {
    *s++ = '0';
    *s++ = 'x';
    for (size_t i = _arg_len; i < 2 * sizeof(void *); i++)
      *s++ = '0';
  }
  while (*arg->text != '\0')
    *s++ = *arg->text++;
  for (size_t i = 0; i < blank_right; i++)
    *s++ = ' ';

  return s;
}

static bool vsprintf_tryfmt(char *_rest *_s, char *sb, cstr _rest *fmt, fmtarg *arg, va_list *va) {
  if (**fmt != '%') return false;
  arg->n = *_s - sb;
  if (!sprint_foramt(arg, fmt, va)) return false;
  *_s = vsprintf_align(*_s, arg);
  return true;
}

dlexport int vsprintf(char *_rest s, cstr _rest fmt, va_list va) {
  if (s == null || fmt == null) return 0;
  char *s_begin = s;

  static fmtarg arg = {.buf = vsprintf_buf, .bufsize = vsprintf_bufsize};

  while (*fmt != '\0') {
    // Warning in x64 ??? WTF
    if (vsprintf_tryfmt(&s, s_begin, &fmt, &arg, &va)) continue;
    *s++ = *fmt++;
  }

  *s = '\0';
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
  return rets;
}

// int vsnprintf(char *_rest s, size_t maxlen, const char *_rest fmt, va_list va) {
//   return 0;
// }

#endif
