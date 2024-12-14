// This code is released under the MIT License

#pragma GCC optimize("O0")

#include <libc-base.h>

#if NO_STD && !USE_SPRINTF

#  define STB_SPRINTF_IMPLEMENTATION
#  include "stb_sprintf.h"

int vsprintf(char *buf, const char *fmt, va_list args) {
  return stbsp_vsprintf(buf, fmt, args);
}

dlexport int sprintf(char *_rest s, cstr _rest fmt, ...) {
  if (fmt == null) return 0;
  va_list va;
  va_start(va, fmt);
  int rets = vsprintf(s, fmt, va);
  va_end(va);
  return rets;
}

#endif
