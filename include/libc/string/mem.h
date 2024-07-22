#pragma once
#include <define.h>
#include <type.h>

#if NO_STD

finline void *memcpy(void *_rest _d, const void *_rest _s, size_t _n) {
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

finline void *memmove(void *_d, const void *_s, size_t _n) {
#  if __has(memmove)
  return __builtin_memmove(_d, _s, _n);
#  else
  byte       *d = _d;
  const byte *s = _s;
  const byte *e = _s + _n;
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
  byte      *s = _s;
  byte      *e = _s + _n;
  const byte c = _c;
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
  // 未完成
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

finline void *memmem(const void *__haystack, size_t __haystacklen, const void *__needle,
                     size_t __needlelen) {}

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

finline void explicit_bzero(void *_s, size_t _n) {
#  if __has(explicit_bzero)
  return __builtin_explicit_bzero(_s, _n);
#  else
#    ifdef __clang__
#      pragma clang optimize off
#    else
#      pragma GCC push_options
#      pragma GCC optimize("O0")
#    endif
  for (size_t i = 0; i < _n; i++) {
    ((byte *)_s)[i] = 0;
  }
#    ifdef __clang__
#      pragma clang optimize on
#    else
#      pragma GCC pop_options
#    endif
#  endif
}

finline void *memfrob(void *_s, size_t _n) {
  for (size_t i = 0; i < _n; i++) {
    ((byte *)_s)[i] ^= 42;
  }
  return _s;
}

#endif
