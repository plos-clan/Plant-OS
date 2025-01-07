#pragma once
#include <define.h>

//* ----------------------------------------------------------------------------------------------------
//; 标准库的
//* ----------------------------------------------------------------------------------------------------
// `` NO_STD ``

#if NO_STD

inline_const int abs(int x) {
  return x < 0 ? -x : x;
}

inline_const float fabsf(float x) {
  return x < 0 ? -x : x;
}

inline_const double fabs(double x) {
  return x < 0 ? -x : x;
}

#  ifndef __cplusplus

#    define abs(x)                                                                                 \
      ({                                                                                           \
        auto _x = (x);                                                                             \
        _x < 0 ? -_x : _x;                                                                         \
      })

#  endif

inline_const float fmodf(float x, float y) {
#  if __has(fmodf)
  return __builtin_fmodf(x, y);
#  else
  return x - (int)(x / y) * y;
#  endif
}

inline_const double fmod(double x, double y) {
  return x - (long)(x / y) * y;
}

inline_const float floorf(float x) {
  return (float)((int)x);
}

inline_const double floor(double x) {
  return (double)((long)x);
}

inline_const float ceilf(float x) {
  return (float)((int)x + ((int)x != x ? 1 : 0));
}

inline_const double ceil(double x) {
  return (double)((long)x + ((long)x != x ? 1 : 0));
}

inline_const float fminf(float x, float y) {
  return x < y ? x : y;
}

inline_const double fmin(double x, double y) {
  return x < y ? x : y;
}

inline_const float fmaxf(float x, float y) {
  return x > y ? x : y;
}

inline_const double fmax(double x, double y) {
  return x > y ? x : y;
}

#endif

//* ----------------------------------------------------------------------------------------------------
//; 自定义的
//* ----------------------------------------------------------------------------------------------------
// `` True ``

// --------------------------------------------------
//; 最大最小

#ifndef __cplusplus

#  define min(a, b)                                                                                \
    ({                                                                                             \
      auto _a = (a);                                                                               \
      auto _b = (b);                                                                               \
      _a < _b ? _a : _b;                                                                           \
    })

#  define max(a, b)                                                                                \
    ({                                                                                             \
      auto _a = (a);                                                                               \
      auto _b = (b);                                                                               \
      _a > _b ? _a : _b;                                                                           \
    })

#endif

// --------------------------------------------------
//; 平方 立方

inline_const int square(int x) {
  return x * x;
}

inline_const float squaref(float x) {
  return x * x;
}

inline_const int cube(int x) {
  return x * x * x;
}

inline_const float cubef(float x) {
  return x * x * x;
}

// --------------------------------------------------
//;

inline_const int gcd(int a, int b) {
  while (b != 0) {
    int t = b;
    b     = a % b;
    a     = t;
  }
  return a;
}

inline_const long gcdl(long a, long b) {
  while (b != 0) {
    long t = b;
    b      = a % b;
    a      = t;
  }
  return a;
}

inline_const u64 factorial(u32 n) {
  u64 result = 1;
  for (u32 i = 2; i <= n; i++) {
    result *= i;
  }
  return result;
}
