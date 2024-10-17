#pragma once
#include <define.h>

// 将字符串转换为数字

#define isdigit2(c)  ((c) == '0' || (c) == '1')
#define isdigit8(c)  ('0' <= (c) && (c) <= '7')
#define isdigit(c)   ('0' <= (c) && (c) <= '9')
#define isdigit16(c) (isdigit(c) || ('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))

#if NO_STD

#  define __ato(t, type)                                                                           \
    finline type ato##t(cstr s) {                                                                  \
      bool neg = *s == '-';                                                                        \
      if (neg || *s == '+') s++;                                                                   \
      type n = 0;                                                                                  \
      for (; isdigit(*s); s++)                                                                     \
        n = n * 10 + (*s - '0');                                                                   \
      return neg ? -n : n;                                                                         \
    }
#  define __uato(t, type)                                                                          \
    finline type ato##t(cstr s) {                                                                  \
      if (*s == '+') s++;                                                                          \
      type n = 0;                                                                                  \
      for (; isdigit(*s); s++)                                                                     \
        n = n * 10 + (*s - '0');                                                                   \
      return n;                                                                                    \
    }

#else

#  define __ato(t, type)  type ato##t(cstr s);
#  define __uato(t, type) type ato##t(cstr s);

#endif

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

#define __atof(_t_, _type_) dlimport _type_ ato##_t_(cstr s);

__atof(ff, float);
__atof(f, double);
__atof(fl, long double);

#define _atof(t) __atof(t, t)

_atof(f32);
_atof(f64);
#if defined(__x86_64__) && !NO_EXTFLOAT
_atof(f16);
_atof(f128);
#endif

#undef _atof

#undef __atof

#define __strto_begin                                                                              \
  cstr __s = s;                                                                                    \
  bool neg = *s == '-';                                                                            \
  if (neg || *s == '+') s++;                                                                       \
  cstr _s = s;

#define __strb2to(t, type)                                                                         \
  finline type strb2to##t(cstr _rest s, char **_rest e) {                                          \
    __strto_begin;                                                                                 \
    type n = 0;                                                                                    \
    if (!isdigit2(*s)) return 0;                                                                   \
    for (; isdigit2(*s); s++)                                                                      \
      n = n * 2 + (*s - '0');                                                                      \
    if (e) *e = (char *)(_s == s ? __s : s);                                                       \
    return neg ? -n : n;                                                                           \
  }
#define __strb8to(t, type)                                                                         \
  finline type strb8to##t(cstr _rest s, char **_rest e) {                                          \
    __strto_begin;                                                                                 \
    type n = 0;                                                                                    \
    for (; isdigit8(*s); s++)                                                                      \
      n = n * 8 + (*s - '0');                                                                      \
    if (e) *e = (char *)(_s == s ? __s : s);                                                       \
    return neg ? -n : n;                                                                           \
  }
#define __strb10to(t, type)                                                                        \
  finline type strb10to##t(cstr _rest s, char **_rest e) {                                         \
    __strto_begin;                                                                                 \
    type n = 0;                                                                                    \
    for (; isdigit(*s); s++)                                                                       \
      n = n * 10 + (*s - '0');                                                                     \
    if (e) *e = (char *)(_s == s ? __s : s);                                                       \
    return neg ? -n : n;                                                                           \
  }
#define __strb16to(t, type)                                                                        \
  finline type strb16to##t(cstr _rest s, char **_rest e) {                                         \
    __strto_begin;                                                                                 \
    type n = 0;                                                                                    \
    for (; isdigit16(*s); s++) {                                                                   \
      if (isdigit(*s)) {                                                                           \
        n = n * 16 + (*s - '0');                                                                   \
      } else if ('a' <= *s && *s <= 'f') {                                                         \
        n = n * 16 + (*s - 'a' + 10);                                                              \
      } else if ('A' <= *s && *s <= 'Z') {                                                         \
        n = n * 16 + (*s - 'A' + 10);                                                              \
      } else {                                                                                     \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    if (e) *e = (char *)(_s == s ? __s : s);                                                       \
    return neg ? -n : n;                                                                           \
  }

#if NO_STD

#  define __strto(t, type)                                                                         \
    finline type strto##t(cstr _rest s, char **_rest e, int base) {                                \
      __strto_begin;                                                                               \
      type n = 0;                                                                                  \
      for (;; s++) {                                                                               \
        if (isdigit(*s)) {                                                                         \
          n = n * base + (*s - '0');                                                               \
        } else if (base > 10) {                                                                    \
          if ('a' <= *s && *s <= 'a' + base - 11) {                                                \
            n = n * base + (*s - 'a' + 10);                                                        \
          } else if ('A' <= *s && *s <= 'A' + base - 11) {                                         \
            n = n * base + (*s - 'A' + 10);                                                        \
          } else {                                                                                 \
            break;                                                                                 \
          }                                                                                        \
        } else {                                                                                   \
          break;                                                                                   \
        }                                                                                          \
      }                                                                                            \
      if (e) *e = (char *)(_s == s ? __s : s);                                                     \
      return neg ? -n : n;                                                                         \
    }

#else

#  define __strto(t, type) type strto##t(cstr _rest s, char **_rest e, int base);

#endif

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
  dlimport _type_ strto##_t_(cstr _rest s, char **_rest e) __THROW __nnull(1)

__stof(f, float);
__stof(d, double);

#define _stof(t) __stof(t, t)

#ifndef NO_STD // TODO 找到声明冲突的原因
_stof(f32);
_stof(f64);
#  if defined(__x86_64__) && !NO_EXTFLOAT
_stof(f16);
_stof(f128);
#  endif
#endif

#undef _stof

#undef __stof

#undef isdigit2
#undef isdigit8
#undef isdigit
#undef isdigit16
