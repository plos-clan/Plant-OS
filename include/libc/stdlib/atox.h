#pragma once
#include "../math.h"
#include <define.h>
#include <type.h>

#define isdigit(c) ('0' <= (c) && (c) <= '9')

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
    if (!isfinite(f)) return f;                                                                    \
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

#undef isdigit
