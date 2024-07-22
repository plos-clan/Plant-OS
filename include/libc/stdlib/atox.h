#pragma once
#include "../math.h"
#include <define.h>
#include <type.h>

#define isdigit2(c)  ((c) == '0' || (c) == '1')
#define isdigit8(c)  ('0' <= (c) && (c) <= '7')
#define isdigit(c)   ('0' <= (c) && (c) <= '9')
#define isdigit16(c) (isdigit(c) || ('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))

#define __ato(t, type)                                                                             \
  finline type ato##t(cstr s) {                                                                    \
    bool neg = *s == '-';                                                                          \
    if (neg || *s == '+') s++;                                                                     \
    type n = 0;                                                                                    \
    for (; isdigit(*s); s++)                                                                       \
      n = n * 10 + (*s - '0');                                                                     \
    return neg ? -n : n;                                                                           \
  }
#define __uato(t, type)                                                                            \
  finline type ato##t(cstr s) {                                                                    \
    if (*s == '+') s++;                                                                            \
    type n = 0;                                                                                    \
    for (; isdigit(*s); s++)                                                                       \
      n = n * 10 + (*s - '0');                                                                     \
    return n;                                                                                      \
  }

__ato(hh, char);
__uato(uhh, uchar);
__ato(h, short);
__uato(uh, ushort);
__ato(i, int);
__uato(ui, uint);
__ato(l, long);
__uato(ul, ulong);
__ato(ll, llong);
__uato(ull, ullong);

#define _ato(t)  __ato(t, t)
#define _uato(t) __uato(t, t)

_ato(i8);
_uato(u8);
_ato(i16);
_uato(u16);
_ato(i32);
_uato(u32);
_ato(i64);
_uato(u64);
#if defined(__x86_64__)
_ato(i128);
_uato(u128);
#endif

#undef _ato
#undef _uato

#undef __ato
#undef __uato

#define __atof(_t_, _type_)                                                                        \
  finline _type_ ato##_t_(cstr s) {                                                                \
    bool neg = *s == '-';                                                                          \
    if (neg || *s == '+') s++;                                                                     \
                                                                                                   \
    if (*s == 'n' || *s == 'N') {                                                                  \
      if (s++, *s == 'a' || *s == 'A')                                                             \
        if (s++, *s == 'n' || *s == 'N') return NAN;                                               \
      return 0;                                                                                    \
    }                                                                                              \
                                                                                                   \
    if (*s == 'i' || *s == 'I') {                                                                  \
      if (s++, *s == 'n' || *s == 'N') {                                                           \
        if (s++, *s == 'f' || *s == 'F') {                                                         \
          if (neg)                                                                                 \
            return -INFINITY;                                                                      \
          else                                                                                     \
            return INFINITY;                                                                       \
        }                                                                                          \
      }                                                                                            \
      return 0;                                                                                    \
    }                                                                                              \
                                                                                                   \
    if (!isdigit(*s) && (*s != '.' || !isdigit(*(s + 1)))) return 0;                               \
                                                                                                   \
    _type_ f = 0;                                                                                  \
                                                                                                   \
    for (; isdigit(*s); s++)                                                                       \
      f = f * 10 + (*s - '0');                                                                     \
                                                                                                   \
    if (!isfinite(f)) return neg ? -f : f;                                                         \
                                                                                                   \
    if (*s == '.') {                                                                               \
      s++;                                                                                         \
      _type_ n = 1;                                                                                \
      for (; isdigit(*s); s++)                                                                     \
        f += (*s - '0') * (n *= (_type_).1);                                                       \
    }                                                                                              \
                                                                                                   \
    if (*s == 'e' || *s == 'E') {                                                                  \
      s++;                                                                                         \
      bool nneg = *s == '-';                                                                       \
      if (nneg || *s == '+') s++;                                                                  \
      u32 n = 0;                                                                                   \
      if (!isdigit(*s)) return 0;                                                                  \
      for (; isdigit(*s); s++)                                                                     \
        n = n * 10 + (*s - '0');                                                                   \
      if (nneg) {                                                                                  \
        _type_ a = .1;                                                                             \
        for (; n; n >>= 1, a *= a)                                                                 \
          if (n & 1) f *= a;                                                                       \
      } else {                                                                                     \
        _type_ a = 10;                                                                             \
        for (; n; n >>= 1, a *= a)                                                                 \
          if (n & 1) f *= a;                                                                       \
      }                                                                                            \
    }                                                                                              \
                                                                                                   \
    return neg ? -f : f;                                                                           \
  }

__atof(ff, float);
__atof(f, double);
__atof(fl, long double);

#define _atof(t) __atof(t, t)

_atof(f32);
_atof(f64);
#if defined(__x86_64__)
_atof(f16);
_atof(f128);
#endif

#undef _atof

#undef __atof

#define __strb2to(t, type)                                                                         \
  finline type strb2to##t(cstr _rest s, char **_rest e) {                                          \
    bool neg = *s == '-';                                                                          \
    if (neg || *s == '+') s++;                                                                     \
    type n = 0;                                                                                    \
    if (!isdigit2(*s) && (*s == '-' || *s == '+')) {                                               \
      *e = (void *)s - 1;                                                                          \
      return 0;                                                                                    \
    }                                                                                              \
    for (; isdigit2(*s); s++)                                                                      \
      n = n * 2 + (*s - '0');                                                                      \
    *e = (void *)s;                                                                                \
    return neg ? -n : n;                                                                           \
  }
#define __strb8to(t, type)                                                                         \
  finline type strb8to##t(cstr _rest s, char **_rest e) {                                          \
    bool neg = *s == '-';                                                                          \
    if (neg || *s == '+') s++;                                                                     \
    type n = 0;                                                                                    \
    if (!isdigit8(*s) && (*s == '-' || *s == '+')) {                                               \
      *e = (void *)s - 1;                                                                          \
      return 0;                                                                                    \
    }                                                                                              \
    for (; isdigit8(*s); s++)                                                                      \
      n = n * 8 + (*s - '0');                                                                      \
    *e = (void *)s;                                                                                \
    return neg ? -n : n;                                                                           \
  }
#define __strb10to(t, type)                                                                        \
  finline type strb10to##t(cstr _rest s, char **_rest e) {                                         \
    bool neg = *s == '-';                                                                          \
    if (neg || *s == '+') s++;                                                                     \
    type n = 0;                                                                                    \
    if (!isdigit(*s) && (*s == '-' || *s == '+')) {                                                \
      *e = (void *)s - 1;                                                                          \
      return 0;                                                                                    \
    }                                                                                              \
    for (; isdigit(*s); s++)                                                                       \
      n = n * 10 + (*s - '0');                                                                     \
    *e = (void *)s;                                                                                \
    return neg ? -n : n;                                                                           \
  }
#define __strb16to(t, type)                                                                        \
  finline type strb16to##t(cstr _rest s, char **_rest e) {                                         \
    bool neg = *s == '-';                                                                          \
    if (neg || *s == '+') s++;                                                                     \
    type n = 0;                                                                                    \
    if (!isdigit16(*s) && (*s == '-' || *s == '+')) {                                              \
      *e = (void *)s - 1;                                                                          \
      return 0;                                                                                    \
    }                                                                                              \
    for (; isdigit16(*s); s++) {                                                                   \
      if (isdigit(*s)) {                                                                           \
        n = n * 16 + (*s - '0');                                                                   \
      } else if ('a' <= *s && *s <= 'f') {                                                         \
        n = n * 16 + (*s - 'a' + 10);                                                              \
      } else {                                                                                     \
        n = n * 16 + (*s - 'A' + 10);                                                              \
      }                                                                                            \
    }                                                                                              \
    *e = (void *)s;                                                                                \
    return neg ? -n : n;                                                                           \
  }
#define __strto(t, type)                                                                           \
  finline type strto##t(cstr _rest s, char **_rest e) {                                            \
    bool neg = *s == '-';                                                                          \
    if (neg || *s == '+') s++;                                                                     \
    type n = 0;                                                                                    \
    if (!isdigit(*s) && (*s == '-' || *s == '+')) {                                                \
      *e = (void *)s - 1;                                                                          \
      return 0;                                                                                    \
    }                                                                                              \
    for (; isdigit(*s); s++)                                                                       \
      n = n * 10 + (*s - '0');                                                                     \
    *e = (void *)s;                                                                                \
    return neg ? -n : n;                                                                           \
  }

#define _strb2to(t)  __strb2to(t, t)
#define _strb8to(t)  __strb8to(t, t)
#define _strb10to(t) __strb10to(t, t)
#define _strb16to(t) __strb16to(t, t)
#define _strto(t)    __strto(t, t)

#define __(name)                                                                                   \
  __##name(hh, char);                                                                              \
  __##name(h, short);                                                                              \
  __##name(i, int);                                                                                \
  __##name(l, long);                                                                               \
  __##name(ll, llong);                                                                             \
  _##name(i8);                                                                                     \
  _##name(i16);                                                                                    \
  _##name(i32);                                                                                    \
  _##name(i64);

__(strb2to);
__(strb8to);
__(strb10to);
__(strb16to);
__(strto);

#undef __

#undef _strb2to
#undef _strb8to
#undef _strb10to
#undef _strb16to
#undef _strto

#undef __strb2to
#undef __strb8to
#undef __strb10to
#undef __strb16to
#undef __strto

#define __stof(_t_, _type_)                                                                        \
  finline _type_ strto##_t_(cstr _rest s, char **_rest e) {                                        \
    bool neg = *s == '-';                                                                          \
    if (neg || *s == '+') s++;                                                                     \
                                                                                                   \
    if (*s == 'n' || *s == 'N') {                                                                  \
      if (s++, *s == 'a' || *s == 'A')                                                             \
        if (s++, *s == 'n' || *s == 'N') {                                                         \
          if (e) *e = (char *)s;                                                                   \
          return NAN;                                                                              \
        }                                                                                          \
      return 0;                                                                                    \
    }                                                                                              \
                                                                                                   \
    if (*s == 'i' || *s == 'I') {                                                                  \
      if (s++, *s == 'n' || *s == 'N') {                                                           \
        if (s++, *s == 'f' || *s == 'F') {                                                         \
          if (e) *e = (char *)s;                                                                   \
          if (neg)                                                                                 \
            return -INFINITY;                                                                      \
          else                                                                                     \
            return INFINITY;                                                                       \
        }                                                                                          \
      }                                                                                            \
      return 0;                                                                                    \
    }                                                                                              \
                                                                                                   \
    if (!isdigit(*s) && (*s != '.' || !isdigit(*(s + 1)))) return 0;                               \
                                                                                                   \
    _type_ f = 0;                                                                                  \
                                                                                                   \
    for (; isdigit(*s); s++)                                                                       \
      f = f * 10 + (*s - '0');                                                                     \
                                                                                                   \
    if (*s == '.') {                                                                               \
      s++;                                                                                         \
      _type_ n = 1;                                                                                \
      for (; isdigit(*s); s++)                                                                     \
        f += (*s - '0') * (n *= (_type_).1);                                                       \
    }                                                                                              \
                                                                                                   \
    if (*s == 'e' || *s == 'E') {                                                                  \
      s++;                                                                                         \
      bool nneg = *s == '-';                                                                       \
      if (nneg || *s == '+') s++;                                                                  \
      u32 n = 0;                                                                                   \
      if (!isdigit(*s)) return 0;                                                                  \
      for (; isdigit(*s); s++)                                                                     \
        n = n * 10 + (*s - '0');                                                                   \
      if (nneg) {                                                                                  \
        _type_ a = .1;                                                                             \
        for (; n; n >>= 1, a *= a)                                                                 \
          if (n & 1) f *= a;                                                                       \
      } else {                                                                                     \
        _type_ a = 10;                                                                             \
        for (; n; n >>= 1, a *= a)                                                                 \
          if (n & 1) f *= a;                                                                       \
      }                                                                                            \
    }                                                                                              \
                                                                                                   \
    if (e) *e = (char *)s;                                                                         \
    return neg ? -f : f;                                                                           \
  }

__stof(ff, float);
__stof(f, double);
__stof(fl, long double);

#define _stof(t) __stof(t, t)

_stof(f32);
_stof(f64);
#if defined(__x86_64__)
_stof(f16);
_stof(f128);
#endif

#undef _stof

#undef __stof

#undef isdigit
