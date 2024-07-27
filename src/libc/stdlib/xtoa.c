#include <libc.h>

// 这些函数将数字转换为字符串形式

#define __tostr_begin                                                                              \
  char *s = buf + len;                                                                             \
  *--s    = '\0';

#define pow_1e9_len 5
static f32 pow_1e9[] = {1, 1e9, 1e18, 1e27, 1e36};

#define pow_1e19_len 17
static f64 pow_1e19[] = {1,     1e19,  1e38,  1e57,  1e76,  1e95,  1e114, 1e133, 1e152,
                         1e171, 1e190, 1e209, 1e228, 1e247, 1e266, 1e285, 1e304};

#define __ftoa(t, type, utype, _e_, _d_)                                                           \
  dlexport cstr t##toa(type n) {                                                                   \
    char *s  = _plos_c_xtoa_buf + _plos_c_xtoa_buf_len;                                            \
    *--s     = '\0';                                                                               \
    bool neg = n < 0;                                                                              \
    if (neg) n = -n;                                                                               \
                                                                                                   \
    if (n == 0) {                                                                                  \
      *--s = '0';                                                                                  \
      if (neg) *--s = '-';                                                                         \
      return s;                                                                                    \
    }                                                                                              \
                                                                                                   \
    utype decimal = (utype)(fmodf(n, 1) * 1e##_d_);                                                \
    if (0 < decimal && decimal < 1e##_d_) {                                                        \
      int i = 0;                                                                                   \
      for (; decimal % 10 == 0; i++)                                                               \
        decimal /= 10;                                                                             \
      for (; i < (_d_); i++) {                                                                     \
        *--s     = decimal % 10 + '0';                                                             \
        decimal /= 10;                                                                             \
      }                                                                                            \
      *--s = '.';                                                                                  \
    }                                                                                              \
                                                                                                   \
    utype it[pow_1e##_e_##_len];                                                                   \
    type  dt[pow_1e##_e_##_len];                                                                   \
    int   cnt;                                                                                     \
    for (cnt = 0; cnt < pow_1e##_e_##_len; cnt++) {                                                \
      dt[cnt] = n / pow_1e##_e_[cnt];                                                              \
      if (dt[cnt] < 1e##_e_) break;                                                                \
    }                                                                                              \
    it[cnt] = (utype)dt[cnt];                                                                      \
    type d  = it[cnt];                                                                             \
    for (int i = cnt - 1; i >= 0; i--) {                                                           \
      d     *= 1e##_e_;                                                                            \
      it[i]  = (utype)dt[i] - d;                                                                   \
      d     += it[i];                                                                              \
    }                                                                                              \
    for (int i = 0; i < cnt; i++) {                                                                \
      utype u = it[i];                                                                             \
      for (int j = 0; j < (_e_); j++) {                                                            \
        *--s  = u % 10 + '0';                                                                      \
        u    /= 10;                                                                                \
      }                                                                                            \
    }                                                                                              \
    for (utype u = it[cnt]; u > 0; u /= 10)                                                        \
      *--s = u % 10 + '0';                                                                         \
                                                                                                   \
    if (neg) *--s = '-';                                                                           \
    return s;                                                                                      \
  }

__ftoa(ff, float, u32, 9, 5);
__ftoa(f, double, u64, 19, 10);
__ftoa(fl, long double, u64, 19, 10);

#define _ftoa(t, ut, e, d) __ftoa(t, t, ut, e, d)

_ftoa(f32, u32, 9, 5);
_ftoa(f64, u64, 19, 5);
#if defined(__x86_64__)
_ftoa(f16, u32, 9, 5);
_ftoa(f128, u64, 19, 5);
#endif

#undef _ftoa

#undef __ftoa

#define __ftostr(t, type, utype, _e_, _d_)                                                         \
  dlexport char *t##tostr(char *buf, size_t len, type n) {                                         \
    __tostr_begin;                                                                                 \
    bool neg = n < 0;                                                                              \
    if (neg) n = -n;                                                                               \
                                                                                                   \
    if (n == 0) {                                                                                  \
      *--s = '0';                                                                                  \
      if (neg) *--s = '-';                                                                         \
      return s;                                                                                    \
    }                                                                                              \
                                                                                                   \
    utype decimal = (utype)(fmodf(n, 1) * 1e##_d_);                                                \
    if (0 < decimal && decimal < 1e##_d_) {                                                        \
      int i = 0;                                                                                   \
      for (; decimal % 10 == 0; i++)                                                               \
        decimal /= 10;                                                                             \
      for (; i < (_d_); i++) {                                                                     \
        *--s     = decimal % 10 + '0';                                                             \
        decimal /= 10;                                                                             \
      }                                                                                            \
      *--s = '.';                                                                                  \
    }                                                                                              \
                                                                                                   \
    utype it[pow_1e##_e_##_len];                                                                   \
    type  dt[pow_1e##_e_##_len];                                                                   \
    int   cnt;                                                                                     \
    for (cnt = 0; cnt < pow_1e##_e_##_len; cnt++) {                                                \
      dt[cnt] = n / pow_1e##_e_[cnt];                                                              \
      if (dt[cnt] < 1e##_e_) break;                                                                \
    }                                                                                              \
    it[cnt] = (utype)dt[cnt];                                                                      \
    type d  = it[cnt];                                                                             \
    for (int i = cnt - 1; i >= 0; i--) {                                                           \
      d     *= 1e##_e_;                                                                            \
      it[i]  = (utype)dt[i] - d;                                                                   \
      d     += it[i];                                                                              \
    }                                                                                              \
    for (int i = 0; i < cnt; i++) {                                                                \
      utype u = it[i];                                                                             \
      for (int j = 0; j < (_e_); j++) {                                                            \
        *--s  = u % 10 + '0';                                                                      \
        u    /= 10;                                                                                \
      }                                                                                            \
    }                                                                                              \
    for (utype u = it[cnt]; u > 0; u /= 10)                                                        \
      *--s = u % 10 + '0';                                                                         \
                                                                                                   \
    if (neg) *--s = '-';                                                                           \
    return s;                                                                                      \
  }

__ftostr(ff, float, u64, 9, 5);
__ftostr(f, double, u64, 19, 10);
__ftostr(fl, long double, u64, 19, 10);

#define _ftostr(t, ut, e, d) __ftostr(t, t, ut, e, d)

_ftostr(f32, u32, 9, 5);
_ftostr(f64, u64, 19, 10);
#if defined(__x86_64__)
_ftostr(f16, u32, 9, 5);
_ftostr(f128, u64, 19, 10);
#endif

#undef _ftostr

#undef __ftostr

#undef __tostr_begin
