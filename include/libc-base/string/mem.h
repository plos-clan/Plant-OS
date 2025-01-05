#pragma once
#include <define.h>

//* ----------------------------------------------------------------------------------------------------
//; 声明
//* ----------------------------------------------------------------------------------------------------

// --------------------------------------------------
//; 标准库函数

#if NO_STD
#  define IAPI finline
#  define OAPI dlimport
#  define SAPI static
#else
#  define IAPI dlimport
#  define OAPI dlimport
#  define SAPI dlimport
#endif

IAPI void *memcpy(void *_rest _d, const void *_rest _s, size_t _n) __THROW __nnull(1, 2);
IAPI void *memmove(void *_d, const void *_s, size_t _n) __THROW __nnull(1, 2);
IAPI void *memset(void *_s, int _c, size_t _n) __THROW __nnull(1);
IAPI int   memcmp(const void *_s1, const void *_s2, size_t _n) __THROW __attr_pure __nnull(1, 2);
#ifdef __cplusplus
extern "C++" {
IAPI const void *memchr(const void *_s, int _c, size_t _n) __THROW __attr_pure __nnull(1);
IAPI void       *memchr(void *_s, int _c, size_t _n) __THROW __attr_pure __nnull(1);
IAPI const void *memrchr(const void *_s, int _c, size_t _n) __THROW __attr_pure __nnull(1);
IAPI void       *memrchr(void *_s, int _c, size_t _n) __THROW __attr_pure __nnull(1);
IAPI const void *rawmemchr(const void *_s, int _c) __THROW;
IAPI void       *rawmemchr(void *_s, int _c) __THROW;
}
#else
IAPI void *memchr(const void *_s, int _c, size_t _n) __THROW __attr_pure __nnull(1);
IAPI void *memrchr(const void *_s, int _c, size_t _n) __THROW __attr_pure __nnull(1);
IAPI void *rawmemchr(const void *_s, int _c) __THROW;
#endif
IAPI void *memccpy(void *_rest _d, const void *_rest _s, int _c, size_t _n) __THROW __nnull(1, 2);
IAPI void *memmem(const void *_s, size_t _sn, const void *_t, size_t _tn) __THROW;
IAPI void *mempcpy(void *_rest _d, const void *_rest _s, size_t _n) __THROW;
IAPI void  bzero(void *_s, size_t _n) __THROW;
SAPI void  explicit_bzero(void *_s, size_t _n) __THROW;
IAPI void *memfrob(void *_s, size_t _n) __THROW __deprecated;
IAPI int   bcmp(const void *s1, const void *s2, size_t n) __THROW __deprecated;

#undef IAPI
#undef OAPI
#undef SAPI

// --------------------------------------------------
//; 非标准库函数

finline u8 *memcpy8(u8 *_rest _d, const u8 *_rest _s, size_t _n) __THROW __nnull(1, 2)
    __attr_readonly(2, 3) __attr_writeonly(1, 3);
finline u16 *memcpy16(u16 *_rest _d, const u16 *_rest _s, size_t _n) __THROW __nnull(1, 2)
    __attr_readonly(2, 3) __attr_writeonly(1, 3);
finline u32 *memcpy32(u32 *_rest _d, const u32 *_rest _s, size_t _n) __THROW __nnull(1, 2)
    __attr_readonly(2, 3) __attr_writeonly(1, 3);
finline u64 *memcpy64(u64 *_rest _d, const u64 *_rest _s, size_t _n) __THROW __nnull(1, 2)
    __attr_readonly(2, 3) __attr_writeonly(1, 3);

finline void *lgmemcpy(void *_rest dst, const void *_rest src, size_t n) __THROW __nnull(1, 2)
    __attr_readonly(2, 3) __attr_writeonly(1, 3);
finline u8 *lgmemcpy8(u8 *_rest dst, const u8 *_rest src, size_t n) __THROW __nnull(1, 2)
    __attr_readonly(2, 3) __attr_writeonly(1, 3);
finline u16 *lgmemcpy16(u16 *_rest dst, const u16 *_rest src, size_t n) __THROW __nnull(1, 2)
    __attr_readonly(2, 3) __attr_writeonly(1, 3);
finline u32 *lgmemcpy32(u32 *_rest dst, const u32 *_rest src, size_t n) __THROW __nnull(1, 2)
    __attr_readonly(2, 3) __attr_writeonly(1, 3);
finline u64 *lgmemcpy64(u64 *_rest dst, const u64 *_rest src, size_t n) __THROW __nnull(1, 2)
    __attr_readonly(2, 3) __attr_writeonly(1, 3);

finline u8  *memset8(u8 *_s, u8 _c, size_t _n) __THROW __nnull(1) __attr_writeonly(1, 3);
finline u16 *memset16(u16 *_s, u16 _c, size_t _n) __THROW __nnull(1) __attr_writeonly(1, 3);
finline u32 *memset32(u32 *_s, u32 _c, size_t _n) __THROW __nnull(1) __attr_writeonly(1, 3);
finline u64 *memset64(u64 *_s, u64 _c, size_t _n) __THROW __nnull(1) __attr_writeonly(1, 3);

finline void *lgmemset(void *_s, byte _c, size_t _n) __THROW __nnull(1) __attr_writeonly(1, 3);
finline u8   *lgmemset8(u8 *s, u8 c, size_t n) __THROW __nnull(1) __attr_writeonly(1, 3);
finline u16  *lgmemset16(u16 *s, u16 c, size_t n) __THROW __nnull(1) __attr_writeonly(1, 3);
finline u32  *lgmemset32(u32 *s, u32 c, size_t n) __THROW __nnull(1) __attr_writeonly(1, 3);
finline u64  *lgmemset64(u64 *s, u64 c, size_t n) __THROW __nnull(1) __attr_writeonly(1, 3);

finline void *memdup(const void *s, size_t n) __nnull(1) __attr_readonly(1, 2) __attr_malloc;

//* ----------------------------------------------------------------------------------------------------
//; 定义
//* ----------------------------------------------------------------------------------------------------

#define MEM_LARGE_SIZE 16384

// --------------------------------------------------
//; 标准库函数

#if NO_STD

finline void *memcpy(void *_rest _d, const void *_rest _s, size_t _n) noexcept {
#  if __has(memcpy)
  return __builtin_memcpy(_d, _s, _n);
#  else
#    if !USE_SSE
  if (_n >= MEM_LARGE_SIZE) return lgmemcpy(_d, _s, _n);
#    endif
  auto d = (volatile byte *)_d;
  auto s = (volatile const byte *)_s;
  auto e = (volatile const byte *)((byte *)_s + _n);
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

finline void *memmove(void *_d, const void *_s, size_t _n) noexcept {
#  if __has(memmove)
  return __builtin_memmove(_d, _s, _n);
#  else
  auto d = (volatile byte *)_d;
  auto s = (volatile const byte *)_s;
  auto e = (volatile const byte *)((byte *)_s + _n);
  __std_safe__({
    if (!d || !s) return null;
    if (d + _n < d || e < s) return null;
  });
  if (d == s) return _d;
  if (d > s && d < e) { // 重叠字符串，反向遍历
    d += _n;
    while (s != e)
      *--d = *--e;
  } else { // 非重叠，正常遍历
    while (s != e)
      *d++ = *s++;
  }
  return _d;
#  endif
}

finline void *memset(void *_s, int _c, size_t _n) noexcept {
#  if __has(memset)
  return __builtin_memset(_s, _c, _n);
#  else
#    if !USE_SSE
  if (_n >= MEM_LARGE_SIZE) return lgmemset(_s, _c, _n);
#    endif
  auto       s = (volatile byte *)_s;
  auto       e = (volatile byte *)((byte *)_s + _n);
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

finline int memcmp(const void *_s1, const void *_s2, size_t _n) noexcept {
#  if __has(memcmp)
  return __builtin_memcmp(_s1, _s2, _n);
#  else
  auto s1 = (const byte *)_s1;
  auto s2 = (const byte *)_s2;
  auto e1 = (const byte *)((byte *)_s1 + _n);
  for (; s1 < e1; s1++, s2++) {
    if (*s1 < *s2) return -1;
    if (*s1 > *s2) return 1;
  }
  return 0;
#  endif
}

#  ifdef __cplusplus // 参考 C 的实现，复制粘贴而已，C++标准非得要重载
extern "C++" {
finline const void *memchr(const void *_s, int _c, size_t _n) noexcept {
#    if __has(memchr)
  return __builtin_memchr(_s, _c, _n);
#    else
  auto       s = (const byte *)_s;
  auto       e = (const byte *)((byte *)_s + _n);
  const byte c = _c;
  for (; s < e; s++)
    if (*s == c) return (void *)s;
  return null;
#    endif
}
finline void *memchr(void *_s, int _c, size_t _n) noexcept {
#    if __has(memchr)
  return __builtin_memchr(_s, _c, _n);
#    else
  auto       s = (const byte *)_s;
  auto       e = (const byte *)((byte *)_s + _n);
  const byte c = _c;
  for (; s < e; s++)
    if (*s == c) return (void *)s;
  return null;
#    endif
}

finline const void *memrchr(const void *_s, int _c, size_t _n) noexcept {
#    if __has(memrchr)
  return __builtin_memrchr(_s, _c, _n);
#    else
  auto       s = (const byte *)_s;
  auto       e = (const byte *)((byte *)_s + _n - 1);
  const byte c = _c;
  __std_safe__({
    if (!s) return null;
  });
  for (; s <= e; e--) {
    if (*e == c) return (void *)e;
  }
  return null;
#    endif
}
finline void *memrchr(void *_s, int _c, size_t _n) noexcept {
#    if __has(memrchr)
  return __builtin_memrchr(_s, _c, _n);
#    else
  auto       s = (const byte *)_s;
  auto       e = (const byte *)((byte *)_s + _n - 1);
  const byte c = _c;
  __std_safe__({
    if (!s) return null;
  });
  for (; s <= e; e--) {
    if (*e == c) return (void *)e;
  }
  return null;
#    endif
}

finline const void *rawmemchr(const void *_s, int _c) noexcept {
  auto       s = (const byte *)_s;
  const byte c = _c;
  __std_safe__({
    if (!s) return null;
  });
  for (; *s != '\0'; s++) {
    if (*s == c) return (void *)s;
  }
  return null;
}
finline void *rawmemchr(void *_s, int _c) noexcept {
  auto       s = (const byte *)_s;
  const byte c = _c;
  __std_safe__({
    if (!s) return null;
  });
  for (; *s != '\0'; s++) {
    if (*s == c) return (void *)s;
  }
  return null;
}
}
#  else
finline void *memchr(const void *_s, int _c, size_t _n) noexcept {
#    if __has(memchr)
  return __builtin_memchr(_s, _c, _n);
#    else
  auto       s = (const byte *)_s;
  auto       e = (const byte *)((byte *)_s + _n);
  const byte c = _c;
  for (; s < e; s++)
    if (*s == c) return (void *)s;
  return null;
#    endif
}

finline void *memrchr(const void *_s, int _c, size_t _n) noexcept {
#    if __has(memrchr)
  return __builtin_memrchr(_s, _c, _n);
#    else
  auto       s = (const byte *)_s;
  auto       e = (const byte *)((byte *)_s + _n - 1);
  const byte c = _c;
  __std_safe__({
    if (!s) return null;
  });
  for (; s <= e; e--) {
    if (*e == c) return (void *)e;
  }
  return null;
#    endif
}

finline void *rawmemchr(const void *_s, int _c) noexcept {
  auto       s = (const byte *)_s;
  const byte c = _c;
  __std_safe__({
    if (!s) return null;
  });
  for (; *s != '\0'; s++) {
    if (*s == c) return (void *)s;
  }
  return null;
}
#  endif

finline void *memccpy(void *_rest _d, const void *_rest _s, int _c, size_t _n) noexcept {
#  if __has(memccpy)
  return __builtin_memccpy(_d, _s, _c, _n);
#  else
  auto d = (byte *)_d;
  auto s = (const byte *)_s;
  auto e = (const byte *)((byte *)_s + _n);
  __std_safe__({
    if (!d || !s) return null;
    if (d + _n < d || e < s) return null;
  });
  if (d == s) return (void *)memchr(_s, _c, _n);
  while (s != e) {
    int c = *d++ = *s++;
    if (c == _c) return d;
  }
  return null;
#  endif
}

finline void *memmem(const void *_s, size_t _sn, const void *_t, size_t _tn) noexcept {
#  if __has(memmem)
  return __builtin_memmem(_s, _sn, _d, _dn);
#  else
  if (_tn == 0) return (void *)_s;
  if (_sn < _tn) return null;
  auto s = (cstr)_s;
  auto t = (cstr)_t;
  for (size_t i = 0; i <= _sn - _tn; i++) {
    if (memcmp(s + i, t, _tn) == 0) return (void *)(s + i);
  }
  return null;
#  endif
}

finline void *mempcpy(void *_rest _d, const void *_rest _s, size_t _n) noexcept {
#  if __has(mempcpy)
  return __builtin_mempcpy(_d, _s, _n);
#  else
  memcpy(_d, _s, _n);
  return (byte *)_d + _n;
#  endif
}

finline void bzero(void *_s, size_t _n) noexcept {
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

#  if !__has(explicit_bzero)
#    ifdef __clang__
#      pragma clang optimize off
#    else
#      pragma GCC push_options
#      pragma GCC optimize("O0")
#    endif
#  endif
static void explicit_bzero(void *_s, size_t _n) noexcept {
#  if __has(explicit_bzero)
  return __builtin_explicit_bzero(_s, _n);
#  else
  for (size_t i = 0; i < _n; i++) {
    ((byte *)_s)[i] = 0;
  }
#  endif
}
#  if !__has(explicit_bzero)
#    ifdef __clang__
#      pragma clang optimize on
#    else
#      pragma GCC pop_options
#    endif
#  endif

finline void *memfrob(void *_s, size_t _n) noexcept {
  for (size_t i = 0; i < _n; i++) {
    ((byte *)_s)[i] ^= 42;
  }
  return _s;
}

finline int bcmp(const void *s1, const void *s2, size_t n) noexcept {
  return memcmp(s1, s2, n);
}

#endif

// --------------------------------------------------
//; 非标准库函数

#if __x86_64__
#  define RAX "rax"
#  define RBX "rbx"
#  define RCX "rcx"
#  define RDX "rdx"
#  define RSI "rsi"
#  define RDI "rdi"
#  define RBP "rbp"
#  define RSP "rsp"
#else
#  define RAX "eax"
#  define RBX "ebx"
#  define RCX "ecx"
#  define RDX "edx"
#  define RSI "esi"
#  define RDI "edi"
#  define RBP "ebp"
#  define RSP "esp"
#endif

finline u8 *memcpy8(u8 *_rest _d, const u8 *_rest _s, size_t _n) noexcept {
#if !USE_SSE
  if (_n >= MEM_LARGE_SIZE) return lgmemcpy8(_d, _s, _n);
#endif
  for (size_t i = 0; i < _n; i++) {
    _d[i] = _s[i];
  }
  return _d;
}
finline u16 *memcpy16(u16 *_rest _d, const u16 *_rest _s, size_t _n) noexcept {
#if !USE_SSE
  if (_n >= MEM_LARGE_SIZE / 2) return lgmemcpy16(_d, _s, _n);
#endif
  for (size_t i = 0; i < _n; i++) {
    _d[i] = _s[i];
  }
  return _d;
}
finline u32 *memcpy32(u32 *_rest _d, const u32 *_rest _s, size_t _n) noexcept {
#if !USE_SSE
  if (_n >= MEM_LARGE_SIZE / 4) return lgmemcpy32(_d, _s, _n);
#endif
  for (size_t i = 0; i < _n; i++) {
    _d[i] = _s[i];
  }
  return _d;
}
finline u64 *memcpy64(u64 *_rest _d, const u64 *_rest _s, size_t _n) noexcept {
#if !USE_SSE
  if (_n >= MEM_LARGE_SIZE / 8) return lgmemcpy64(_d, _s, _n);
#endif
  for (size_t i = 0; i < _n; i++) {
    _d[i] = _s[i];
  }
  return _d;
}

finline void *lgmemcpy(void *_rest dst, const void *_rest src, size_t n) noexcept {
  return (void *)lgmemcpy8((u8 *)dst, (const u8 *)src, n);
}
finline u8 *lgmemcpy8(u8 *_rest dst, const u8 *_rest src, size_t n) noexcept {
#if USE_SSE
  return memcpy8(dst, src, n);
#else
  size_t _1, _2;
  asm volatile("rep movsb"
               : "=D"(_1), "=S"(_2)
               : "D"((size_t)dst), "S"((size_t)src), "c"(n)
               : "memory");
  return dst;
#endif
}
finline u16 *lgmemcpy16(u16 *_rest dst, const u16 *_rest src, size_t n) noexcept {
#if USE_SSE
  return memcpy16(dst, src, n);
#else
  size_t _1, _2;
  asm volatile("rep movsw"
               : "=D"(_1), "=S"(_2)
               : "D"((size_t)dst), "S"((size_t)src), "c"(n)
               : "memory");
  return dst;
#endif
}
finline u32 *lgmemcpy32(u32 *_rest dst, const u32 *_rest src, size_t n) noexcept {
#if USE_SSE
  return memcpy32(dst, src, n);
#else
  size_t _1, _2;
  asm volatile("rep movsl"
               : "=D"(_1), "=S"(_2)
               : "D"((size_t)dst), "S"((size_t)src), "c"(n)
               : "memory");
  return dst;
#endif
}
finline u64 *lgmemcpy64(u64 *_rest dst, const u64 *_rest src, size_t n) noexcept {
#if USE_SSE
  return memcpy64(dst, src, n);
#else
#  if __x86_64__
  size_t _1, _2;
  asm volatile("rep movsq"
               : "=D"(_1), "=S"(_2)
               : "D"((size_t)dst), "S"((size_t)src), "c"(n)
               : "memory");
  return dst;
#  else
  return (u64 *)lgmemcpy32((u32 *)dst, (u32 *)src, n * 2);
#  endif
#endif
}

finline u8 *memset8(u8 *_s, u8 _c, size_t _n) noexcept {
#if !USE_SSE
  if (_n >= MEM_LARGE_SIZE) return lgmemset8(_s, _c, _n);
#endif
  for (size_t i = 0; i < _n; i++) {
    _s[i] = _c;
  }
  return _s;
}
finline u16 *memset16(u16 *_s, u16 _c, size_t _n) noexcept {
#if !USE_SSE
  if (_n >= MEM_LARGE_SIZE / 2) return lgmemset16(_s, _c, _n);
#endif
  for (size_t i = 0; i < _n; i++) {
    _s[i] = _c;
  }
  return _s;
}
finline u32 *memset32(u32 *_s, u32 _c, size_t _n) noexcept {
#if !USE_SSE
  if (_n >= MEM_LARGE_SIZE / 4) return lgmemset32(_s, _c, _n);
#endif
  for (size_t i = 0; i < _n; i++) {
    _s[i] = _c;
  }
  return _s;
}
finline u64 *memset64(u64 *_s, u64 _c, size_t _n) noexcept {
#if !USE_SSE
  if (_n >= MEM_LARGE_SIZE / 8) return lgmemset64(_s, _c, _n);
#endif
  for (size_t i = 0; i < _n; i++) {
    _s[i] = _c;
  }
  return _s;
}

finline void *lgmemset(void *_s, byte _c, size_t _n) noexcept {
  return (void *)lgmemset8((u8 *)_s, _c, _n);
}
finline u8 *lgmemset8(u8 *s, u8 c, size_t n) noexcept {
#if USE_SSE
  return memset8(s, c, n);
#else
  size_t _1, _2;
  asm volatile("rep stosb" : "=D"(_1), "=c"(_2) : "D"((size_t)s), "a"(c), "c"(n) : "memory");
  return s;
#endif
}
finline u16 *lgmemset16(u16 *s, u16 c, size_t n) noexcept {
#if USE_SSE
  return memset16(s, c, n);
#else
  size_t _1, _2;
  asm volatile("rep stosw" : "=D"(_1), "=c"(_2) : "D"((size_t)s), "a"(c), "c"(n) : "memory");
  return s;
#endif
}
finline u32 *lgmemset32(u32 *s, u32 c, size_t n) noexcept {
#if USE_SSE
  return memset32(s, c, n);
#else
  size_t _1, _2;
  asm volatile("rep stosl" : "=D"(_1), "=c"(_2) : "D"((size_t)s), "a"(c), "c"(n) : "memory");
  return s;
#endif
}
finline u64 *lgmemset64(u64 *s, u64 c, size_t n) noexcept {
#if USE_SSE
  return memset64(s, c, n);
#else
#  if __x86_64__ && !USE_SSE
  size_t _1, _2;
  asm volatile("rep stosq" : "=D"(_1), "=c"(_2) : "D"((size_t)s), "a"(c), "c"(n) : "memory");
#  else
  for (size_t i = 0; i < n; i++) {
    s[i] = c;
  }
#  endif
  return s;
#endif
}
