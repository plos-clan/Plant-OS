#pragma once
#include <define.h>

//* ----------------------------------------------------------------------------------------------------
//; 标准库的
//* ----------------------------------------------------------------------------------------------------

#if NO_STD

#  ifndef __cplusplus

#    define abs(x)                                                                                 \
      ({                                                                                           \
        auto _x = (x);                                                                             \
        _x < 0 ? -_x : _x;                                                                         \
      })

#  endif

finline float fmodf(float x, float y) {
  return x - (int)(x / y) * y;
}

finline double fmod(double x, double y) {
  return x - (long)(x / y) * y;
}

finline float floorf(float x) {
  return (float)((int)x);
}

finline double floor(double x) {
  return (double)((long)x);
}

finline float ceilf(float x) {
  return (float)((int)x + 1);
}

finline double ceil(double x) {
  return (double)((long)x + 1);
}

finline float fminf(float x, float y) {
  return x < y ? x : y;
}

finline double fmin(double x, double y) {
  return x < y ? x : y;
}

finline float fmaxf(float x, float y) {
  return x > y ? x : y;
}

finline double fmax(double x, double y) {
  return x > y ? x : y;
}

#endif

//* ----------------------------------------------------------------------------------------------------
//; 自定义的
//* ----------------------------------------------------------------------------------------------------

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

finline int square(int x) {
  return x * x;
}

finline float squaref(float x) {
  return x * x;
}

finline int cube(int x) {
  return x * x * x;
}

finline float cubef(float x) {
  return x * x * x;
}

// --------------------------------------------------
//;

finline int gcd(int a, int b) {
  while (b != 0) {
    int t = b;
    b     = a % b;
    a     = t;
  }
  return a;
}

finline long gcdl(long a, long b) {
  while (b != 0) {
    long t = b;
    b      = a % b;
    a      = t;
  }
  return a;
}

finline u64 factorial(u32 n) {
  u64 result = 1;
  for (u32 i = 2; i <= n; i++) {
    result *= i;
  }
  return result;
}
