#pragma once
#include <define.h>
#include <type.h>

#define _plos_c_xtoa_buf_len 32
extern char _plos_c_xtoa_buf[_plos_c_xtoa_buf_len];

extern const char _plos_lut_alnum_lower[62];
extern const char _plos_lut_alnum_upper[62];

// 这些函数将数字转换为字符串形式

#define __toa(t, type)                                                                             \
  finline cstr t##toa(type n) {                                                                    \
    char *s  = _plos_c_xtoa_buf + _plos_c_xtoa_buf_len;                                            \
    *--s     = '\0';                                                                               \
    bool neg = n < 0;                                                                              \
    if (neg) n = -n;                                                                               \
    if (n == 0) *--s = '0';                                                                        \
    for (; n > 0; n /= 10)                                                                         \
      *--s = n % 10 + '0';                                                                         \
    if (neg) *--s = '-';                                                                           \
    return s;                                                                                      \
  }
#define __utoa(t, type)                                                                            \
  finline cstr t##toa(type n) {                                                                    \
    char *s = _plos_c_xtoa_buf + _plos_c_xtoa_buf_len;                                             \
    *--s    = '\0';                                                                                \
    if (n == 0) *--s = '0';                                                                        \
    for (; n > 0; n /= 10)                                                                         \
      *--s = n % 10 + '0';                                                                         \
    return s;                                                                                      \
  }

#define _toa(t)  __toa(t, t)
#define _utoa(t) __utoa(t, t)

__toa(hh, char);
__utoa(uhh, unsigned char);
__toa(h, short);
__utoa(uh, unsigned short);
__toa(i, int);
__utoa(ui, unsigned int);
__toa(l, long int);
__utoa(ul, unsigned long int);
__toa(ll, long long int);
__utoa(ull, unsigned long long int);

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

#define __tostr(t, type, utype)                                                                    \
  finline char *t##tostr(char *buf, size_t len, type _n, int b) {                                  \
    __tostr_begin_nlt;                                                                             \
    if (_n == 0) return (*--s = '0', s);                                                           \
    bool  neg = _n < 0;                                                                            \
    utype n   = neg ? -_n : _n;                                                                    \
    for (; n; n /= b)                                                                              \
      *--s = nlt[n % b];                                                                           \
    if (neg) *--s = '-';                                                                           \
    return s;                                                                                      \
  }
#define __utostr(t, type)                                                                          \
  finline char *t##tostr(char *buf, size_t len, type n, int b) {                                   \
    __tostr_begin_nlt;                                                                             \
    if (n == 0) return (*--s = '0', s);                                                            \
    for (; n; n /= b)                                                                              \
      *--s = nlt[n % b];                                                                           \
    return s;                                                                                      \
  }

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

#undef __tostr_begin
#undef __tostr_begin_nlt
