#pragma once
#include <define.h>

#define _plos_c_xtoa_buf_len 32
extern char _plos_c_xtoa_buf[_plos_c_xtoa_buf_len];

// 查找表：0123456789abcd...
extern const char _plos_lut_alnum_lower[62];
// 查找表：0123456789ABCD...
extern const char _plos_lut_alnum_upper[62];

// 这些函数将数字转换为字符串形式

#if NO_STD

#  define __toa(t, type)                                                                           \
    finline cstr t##toa(type n) {                                                                  \
      char *s  = _plos_c_xtoa_buf + _plos_c_xtoa_buf_len;                                          \
      *--s     = '\0';                                                                             \
      bool neg = n < 0;                                                                            \
      if (neg) n = -n;                                                                             \
      if (n == 0) *--s = '0';                                                                      \
      for (; n > 0; n /= 10)                                                                       \
        *--s = n % 10 + '0';                                                                       \
      if (neg) *--s = '-';                                                                         \
      return s;                                                                                    \
    }
#  define __utoa(t, type)                                                                          \
    finline cstr t##toa(type n) {                                                                  \
      char *s = _plos_c_xtoa_buf + _plos_c_xtoa_buf_len;                                           \
      *--s    = '\0';                                                                              \
      if (n == 0) *--s = '0';                                                                      \
      for (; n > 0; n /= 10)                                                                       \
        *--s = n % 10 + '0';                                                                       \
      return s;                                                                                    \
    }

#else

#  define __toa(t, type)  cstr t##toa(type n);
#  define __utoa(t, type) cstr t##toa(type n);

#endif

#define _toa(t)  __toa(t, t)
#define _utoa(t) __utoa(t, t)

__toa(hh, char);
__utoa(uhh, uchar);
__toa(h, short);
__utoa(uh, ushort);
__toa(i, int);
__utoa(ui, uint);
__toa(l, long);
__utoa(ul, ulong);
__toa(ll, llong);
__utoa(ull, ullong);

_toa(i8);
_utoa(u8);
_toa(i16);
_utoa(u16);
_toa(i32);
_utoa(u32);
_toa(i64);
_utoa(u64);

#undef _utoa
#undef _toa

#undef __utoa
#undef __toa

#define __tostr_begin                                                                              \
  char *s = buf + len;                                                                             \
  *--s    = '\0';
#define __tostr_begin_nlt                                                                          \
  cstr nlt = _plos_lut_alnum_upper;                                                                \
  if (b < 0) (b = -b, nlt = _plos_lut_alnum_lower);                                                \
  __tostr_begin

#define __tostrb2(t, type, utype)                                                                  \
  finline char *t##tostrb2(char *buf, size_t len, type _n) {                                       \
    __tostr_begin;                                                                                 \
    if (_n == 0) return (*--s = '0', s);                                                           \
    bool  neg = _n < 0;                                                                            \
    utype n   = neg ? -_n : _n;                                                                    \
    for (; n; n >>= 1)                                                                             \
      *--s = (n & 1) + '0';                                                                        \
    if (neg) *--s = '-';                                                                           \
    return s;                                                                                      \
  }
#define __utostrb2(t, type)                                                                        \
  finline char *t##tostrb2(char *buf, size_t len, type n) {                                        \
    __tostr_begin;                                                                                 \
    if (n == 0) return (*--s = '0', s);                                                            \
    for (; n; n >>= 1)                                                                             \
      *--s = (n & 1) + '0';                                                                        \
    return s;                                                                                      \
  }

#define __tostrb8(t, type, utype)                                                                  \
  finline char *t##tostrb8(char *buf, size_t len, type _n) {                                       \
    __tostr_begin;                                                                                 \
    if (_n == 0) return (*--s = '0', s);                                                           \
    bool  neg = _n < 0;                                                                            \
    utype n   = neg ? -_n : _n;                                                                    \
    for (; n; n >>= 3)                                                                             \
      *--s = (n & 7) + '0';                                                                        \
    if (neg) *--s = '-';                                                                           \
    return s;                                                                                      \
  }
#define __utostrb8(t, type)                                                                        \
  finline char *t##tostrb8(char *buf, size_t len, type n) {                                        \
    __tostr_begin;                                                                                 \
    if (n == 0) return (*--s = '0', s);                                                            \
    for (; n; n >>= 3)                                                                             \
      *--s = (n & 7) + '0';                                                                        \
    return s;                                                                                      \
  }

#define __tostrb10(t, type, utype)                                                                 \
  finline char *t##tostrb10(char *buf, size_t len, type _n) {                                      \
    __tostr_begin;                                                                                 \
    if (_n == 0) return (*--s = '0', s);                                                           \
    bool  neg = _n < 0;                                                                            \
    utype n   = neg ? -_n : _n;                                                                    \
    for (; n; n /= 10)                                                                             \
      *--s = n % 10 + '0';                                                                         \
    if (neg) *--s = '-';                                                                           \
    return s;                                                                                      \
  }
#define __utostrb10(t, type)                                                                       \
  finline char *t##tostrb10(char *buf, size_t len, type n) {                                       \
    __tostr_begin;                                                                                 \
    if (n == 0) return (*--s = '0', s);                                                            \
    for (; n; n /= 10)                                                                             \
      *--s = n % 10 + '0';                                                                         \
    return s;                                                                                      \
  }

#define __tostrb16(t, type, utype)                                                                 \
  finline char *t##tostrb16(char *buf, size_t len, type _n) {                                      \
    __tostr_begin;                                                                                 \
    if (_n == 0) return (*--s = '0', s);                                                           \
    bool  neg = _n < 0;                                                                            \
    utype n   = neg ? -_n : _n;                                                                    \
    for (; n; n >>= 4)                                                                             \
      *--s = _plos_lut_alnum_lower[n & 15];                                                        \
    if (neg) *--s = '-';                                                                           \
    return s;                                                                                      \
  }
#define __utostrb16(t, type)                                                                       \
  finline char *t##tostrb16(char *buf, size_t len, type n) {                                       \
    __tostr_begin;                                                                                 \
    if (n == 0) return (*--s = '0', s);                                                            \
    for (; n; n >>= 4)                                                                             \
      *--s = _plos_lut_alnum_lower[n & 15];                                                        \
    return s;                                                                                      \
  }

#define __tostrB16(t, type, utype)                                                                 \
  finline char *t##tostrB16(char *buf, size_t len, type _n) {                                      \
    __tostr_begin;                                                                                 \
    if (_n == 0) return (*--s = '0', s);                                                           \
    bool  neg = _n < 0;                                                                            \
    utype n   = neg ? -_n : _n;                                                                    \
    for (; n; n >>= 4)                                                                             \
      *--s = _plos_lut_alnum_upper[n & 15];                                                        \
    if (neg) *--s = '-';                                                                           \
    return s;                                                                                      \
  }
#define __utostrB16(t, type)                                                                       \
  finline char *t##tostrB16(char *buf, size_t len, type n) {                                       \
    __tostr_begin;                                                                                 \
    if (n == 0) return (*--s = '0', s);                                                            \
    for (; n; n >>= 4)                                                                             \
      *--s = _plos_lut_alnum_upper[n & 15];                                                        \
    return s;                                                                                      \
  }

#if NO_STD

#  define __tostr(t, type, utype)                                                                  \
    finline char *t##tostr(char *buf, size_t len, type _n, int b) {                                \
      __tostr_begin_nlt;                                                                           \
      if (_n == 0) return (*--s = '0', s);                                                         \
      bool  neg = _n < 0;                                                                          \
      utype n   = neg ? -_n : _n;                                                                  \
      for (; n; n /= b)                                                                            \
        *--s = nlt[n % b];                                                                         \
      if (neg) *--s = '-';                                                                         \
      return s;                                                                                    \
    }
#  define __utostr(t, type)                                                                        \
    finline char *t##tostr(char *buf, size_t len, type n, int b) {                                 \
      __tostr_begin_nlt;                                                                           \
      if (n == 0) return (*--s = '0', s);                                                          \
      for (; n; n /= b)                                                                            \
        *--s = nlt[n % b];                                                                         \
      return s;                                                                                    \
    }

#else

#  define __tostr(t, type, utype) char *t##tostr(char *buf, size_t len, type _n, int b);
#  define __utostr(t, type)       char *t##tostr(char *buf, size_t len, type n, int b);

#endif

#define _tostrb2(t, u)  __tostrb2(t, t, u)
#define _utostrb2(t)    __utostrb2(t, t)
#define _tostrb8(t, u)  __tostrb8(t, t, u)
#define _utostrb8(t)    __utostrb8(t, t)
#define _tostrb10(t, u) __tostrb10(t, t, u)
#define _utostrb10(t)   __utostrb10(t, t)
#define _tostrb16(t, u) __tostrb16(t, t, u)
#define _utostrb16(t)   __utostrb16(t, t)
#define _tostrB16(t, u) __tostrB16(t, t, u)
#define _utostrB16(t)   __utostrB16(t, t)
#define _tostr(t, u)    __tostr(t, t, u)
#define _utostr(t)      __utostr(t, t)

#define __(name)                                                                                   \
  __##name(hh, char, uchar);                                                                       \
  __u##name(uhh, uchar);                                                                           \
  __##name(h, short, ushort);                                                                      \
  __u##name(uh, ushort);                                                                           \
  __##name(i, int, uint);                                                                          \
  __u##name(ui, uint);                                                                             \
  __##name(l, long, ulong);                                                                        \
  __u##name(ul, ulong);                                                                            \
  __##name(ll, llong, ullong);                                                                     \
  __u##name(ull, ullong);                                                                          \
  _##name(i8, u8);                                                                                 \
  _u##name(u8);                                                                                    \
  _##name(i16, u16);                                                                               \
  _u##name(u16);                                                                                   \
  _##name(i32, u32);                                                                               \
  _u##name(u32);                                                                                   \
  _##name(i64, u64);                                                                               \
  _u##name(u64);

__(tostrb2);
__(tostrb8);
__(tostrb10);
__(tostrb16);
__(tostrB16);
__(tostr);

#undef __

#undef _tostrb2
#undef _utostrb2
#undef _tostrb8
#undef _utostrb8
#undef _tostrb10
#undef _utostrb10
#undef _tostrb16
#undef _utostrb16
#undef _tostrB16
#undef _utostrB16
#undef _tostr
#undef _utostr

#undef __tostrb2
#undef __utostrb2
#undef __tostrb8
#undef __utostrb8
#undef __tostrb10
#undef __utostrb10
#undef __tostrb16
#undef __utostrb16
#undef __tostrB16
#undef __utostrB16
#undef __tostr
#undef __utostr

#define __ftostr(t, type, utype, _e_) dlimport char *t##tostr(char *buf, size_t len, type n);

__ftostr(ff, float, u64, 9);
__ftostr(f, double, u64, 19);
__ftostr(fl, long double, u64, 19);

#define _ftostr(t, ut, e) __ftostr(t, t, ut, e)

_ftostr(f32, u32, 9);
_ftostr(f64, u64, 19);
#if defined(__x86_64__) && !NO_EXTFLOAT
_ftostr(f16, u32, 9);
_ftostr(f128, u64, 19);
#endif

#undef _ftostr

#undef __ftostr

#define __ftoa(t, type, utype, _e_) dlimport cstr t##toa(type n);

__ftoa(ff, float, u32, 9);
__ftoa(f, double, u64, 19);
__ftoa(fl, long double, u64, 19);

#define _ftoa(t, ut, e) __ftoa(t, t, ut, e)

_ftoa(f32, u32, 9);
_ftoa(f64, u64, 19);
#if defined(__x86_64__) && !NO_EXTFLOAT
_ftoa(f16, u32, 9);
_ftoa(f128, u64, 19);
#endif

#undef _ftoa

#undef __ftoa

#undef __tostr_begin
#undef __tostr_begin_nlt
