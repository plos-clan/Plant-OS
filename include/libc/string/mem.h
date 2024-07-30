#pragma once
#include "libc/asm/asm.h"
#include <define.h>
#include <type.h>

// 声明

finline void *memcpy(void *_rest _d, const void *_rest _s, size_t _n);
finline void *memmove(void *_d, const void *_s, size_t _n);
finline void *memset(void *_s, int _c, size_t _n);
finline int   memcmp(const void *_s1, const void *_s2, size_t _n);
finline void *memchr(const void *_s, int _c, size_t _n);
finline void *memccpy(void *_rest _d, const void *_rest _s, int _c, size_t _n);
finline void *rawmemchr(const void *_s, int _c);
finline void *memrchr(const void *_s, int _c, size_t _n);
finline void *memmem(const void *_s, size_t _sn, const void *_t, size_t _tn);
finline void *mempcpy(void *_rest _d, const void *_rest _s, size_t _n);
finline void  bzero(void *_s, size_t _n);
finline void  explicit_bzero(void *_s, size_t _n);
finline void *memfrob(void *_s, size_t _n);

// 非标准库函数
finline void *memdup(const void *_s, size_t _n);

// 定义

#if NO_STD

finline void *memcpy(void *_rest _d, const void *_rest _s, size_t _n) {
#  if __has(memcpy)
  return __builtin_memcpy(_d, _s, _n);
#  else
  volatile byte       *d = _d;
  volatile const byte *s = _s;
  volatile const byte *e = _s + _n;
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

finline void *memmove(void *_d, const void *_s, size_t _n) {
#  if __has(memmove)
  return __builtin_memmove(_d, _s, _n);
#  else
  volatile byte       *d = _d;
  volatile const byte *s = _s;
  volatile const byte *e = _s + _n;
  __std_safe__({
    if (!d || !s) return null;
    if (d + _n < d || e < s) return null;
  });
  if (d == s) return _d;
  if (d > s && d < e) { // 重叠字符串，反向遍历
    d += _n;
    while (s != e)
      *d-- = *e--;
  } else { // 非重叠，正常遍历
    while (s != e)
      *d++ = *s++;
  }
  return _d;
#  endif
}

finline void *memset(void *_s, int _c, size_t _n) {
#  if __has(memset)
  return __builtin_memset(_s, _c, _n);
#  else
  volatile byte *s = _s;
  volatile byte *e = _s + _n;
  const byte     c = _c;
  __std_safe__({
    if (!s) return null;
    if (e < s) return null;
  });
  for (; s < e; s++)
    *s = c;
  return _s;
#  endif
}

finline int memcmp(const void *_s1, const void *_s2, size_t _n) {
#  if __has(memcmp)
  return __builtin_memcmp(_s1, _s2, _n);
#  else
  const byte *s1 = _s1, *s2 = _s2;
  const byte *e1 = _s1 + _n;
  for (; s1 < e1; s1++, s2++) {
    if (*s1 < *s2) return -1;
    if (*s1 > *s2) return 1;
  }
  return 0;
#  endif
}

finline void *memchr(const void *_s, int _c, size_t _n) {
#  if __has(memchr)
  return __builtin_memchr(_s, _c, _n);
#  else
  const byte *s = _s;
  const byte *e = _s + _n;
  const byte  c = _c;
  for (; s < e; s++)
    if (*s == c) return (void *)s;
  return null;
#  endif
}

finline void *memccpy(void *_rest _d, const void *_rest _s, int _c, size_t _n) {
#  if __has(memccpy)
  return __builtin_memccpy(_d, _s, _c, _n);
#  else
  byte       *d = _d;
  const byte *s = _s;
  const byte *e = _s + _n;
  __std_safe__({
    if (!d || !s) return null;
    if (d + _n < d || e < s) return null;
  });
  if (d == s) return memchr(_s, _c, _n);
  while (s != e) {
    int c = *d++ = *s++;
    if (c == _c) return d;
  }
  return null;
#  endif
}

finline void *rawmemchr(const void *_s, int _c) {
  const byte *s = _s;
  const byte  c = _c;
  __std_safe__({
    if (!s) return null;
  });
  for (; *s != '\0'; s++) {
    if (*s == c) return (void *)s;
  }
  return null;
}

finline void *memrchr(const void *_s, int _c, size_t _n) {
#  if __has(memrchr)
  return __builtin_memrchr(_s, _c, _n);
#  else
  const byte *s = _s;
  const byte *e = _s + _n - 1;
  const byte  c = _c;
  __std_safe__({
    if (!s) return null;
  });
  for (; s <= e; e--) {
    if (*e == c) return (void *)e;
  }
  return null;
#  endif
}

finline void *memmem(const void *_s, size_t _sn, const void *_t, size_t _tn) {
#  if __has(memmem)
  return __builtin_memmem(_s, _sn, _d, _dn);
#  else
  if (_tn == 0) return (void *)_s;
  if (_sn < _tn) return null;
  cstr s = _s;
  cstr t = _t;
  for (size_t i = 0; i <= _sn - _tn; i++) {
    if (memcmp(s + i, t, _tn) == 0) return (void *)(s + i);
  }
  return null;
#  endif
}

finline void *mempcpy(void *_rest _d, const void *_rest _s, size_t _n) {
#  if __has(mempcpy)
  return __builtin_mempcpy(_d, _s, _n);
#  else
  memcpy(_d, _s, _n);
  return (byte *)_d + _n;
#  endif
}

finline void bzero(void *_s, size_t _n) {
#  if __has(bzero)
  return __builtin_bzero(_s, _n);
#  elif __has(memset)
  return __builtin_memset(_s, 0, _n);
#  else
  for (size_t i = 0; i < _n; i++) {
    ((byte *)_s)[i] = 0;
  }
#  endif
}

#  if !__has(explicit_bzero) && !defined(__clang__)
#    pragma GCC push_options
#    pragma GCC optimize("O0")
#  endif
finline void explicit_bzero(void *_s, size_t _n) {
#  if __has(explicit_bzero)
  return __builtin_explicit_bzero(_s, _n);
#  else
#    ifdef __clang__
#      pragma clang optimize off
#    endif
  for (size_t i = 0; i < _n; i++) {
    ((byte *)_s)[i] = 0;
  }
#    ifdef __clang__
#      pragma clang optimize on
#    endif
#  endif
}
#  if !__has(explicit_bzero) && !defined(__clang__)
#    pragma GCC pop_options
#  endif

finline void *memfrob(void *_s, size_t _n) {
  for (size_t i = 0; i < _n; i++) {
    ((byte *)_s)[i] ^= 42;
  }
  return _s;
}

#endif
