#include "libc/string/mem.h"

// 如果是 GCC 则使用内联版本
// 如果是 Clang 则使用这里的定义
#if __clang__
void *memcpy(void *_rest _d, const void *_rest _s, size_t _n) {
#  if __has(memcpy)
  return __builtin_memcpy(_d, _s, _n);
#  else
  byte       *d = _d;
  const byte *s = _s;
  const byte *e = _s + _n;
  __std_safe__({
    if (!d || !s) return null;
    if (d + _n < d || e < s) return null;
  });
  if (d == s) return _d;
  while (s != e)
    *d++ = *s++;
  return _d;
#  endif
}
#endif