#include <libc.h>

const char *sprint_foramt(struct print_fmtarg *__restrict arg, const char *__restrict fmt,
                          va_list va) {
  arg->align   = 0;
  arg->setfg   = false;
  arg->setfg   = false;
  arg->decimal = 0;
  arg->maxlen  = 0;
  arg->minlen  = 0;
  memset(arg->buf, 0, arg->bufsize);
  arg->text = arg->buf;

  fmt++;

  if (*fmt == '%') {
    *arg->text = '%';
    return fmt + 1;
  }

  char *_fmt;
  arg->maxlen = strb10toi(fmt, &_fmt);
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
    arg->decimal = strb10toi(fmt, &_fmt);
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

  while (1)
    switch (*fmt++) {

    case 'h':
      if (0 < _size && _size <= 3) {
        _size--;
        continue;
      }
      return NULL; // 解析失败

    case 'l':
      if (3 <= _size && _size < 6) {
        _size++;
        continue;
      }
      return NULL; // 解析失败

    case 'c':
    case 'C':
      if (_size != 3) return NULL; // 解析失败
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

    case 'n': // 字符串
    case 'N':
      if (_size != 3) return NULL; // 解析失败
      int *np = va_arg(va, int *);
      *np     = arg->n;
      goto end;

    case 's': // 字符串
    case 'S':
      if (_size != 3) return NULL; // 解析失败
      arg->text = va_arg(va, void *);
      if (!arg->text) arg->text = arg->buf;
      goto end;

    default: return NULL; // 解析失败
    }

end:
  arg->len = strlen(arg->text);
  return fmt;
}

int vsprintf(char *__restrict s, const char *__restrict fmt, va_list va) {
  if (!s) return 0;
  if (!fmt) return 0;

  char *__s = s;

  char                buf[1024];
  struct print_fmtarg arg = {.buf = buf, .bufsize = 1024};

  while (*fmt) {
    if (*fmt == '%') {
      arg.n            = s - __s;
      const char *_fmt = sprint_foramt(&arg, fmt, va);
      if (_fmt) {
        fmt = _fmt;
        // 长度超过了最大限制
        if (arg.maxlen && arg.len >= arg.maxlen) {
          for (int i = 0; i < arg.maxlen; i++)
            *s++ = arg.text[i];
        }

        int blank = arg.minlen - (int)arg.len;

        if (blank > 0)
          if (arg.align == 0) // 靠右对齐
            for (int i = 0; i < blank; i++)
              *s++ = ' ';
          else if (arg.align == 2) // 居中对齐
            for (int i = 0; i < blank / 2; i++)
              *s++ = ' ';

        while (*arg.text)
          *s++ = *arg.text++;

        if (blank > 0)
          if (arg.align == 1) // 靠左对齐
            for (int i = 0; i < blank; i++)
              *s++ = ' ';
          else if (arg.align == 2) // 居中对齐
            for (int i = 0; i < (blank + 1) / 2; i++)
              *s++ = ' ';

        continue;
      }
    }
    *s++ = *fmt++;
  }

  return s - __s;
}

int sprintf(char *__restrict s, const char *__restrict fmt, ...) {
  if (!s) return 0;
  if (!fmt) return 0;
  va_list va;
  va_start(va, fmt);
  int rets = vsprintf(s, fmt, va);
  va_end(va);
  return rets;
}

ssize_t sformat(char *_rest _s, cstr _rest _fmt) {}

int vsnprintf(char *__restrict s, size_t maxlen, const char *__restrict fmt, va_list va) {}
