#pragma once
#include "base.h"
#include "math.h"
#ifdef __cplusplus
extern "C++" {
#  include "base.hpp"
}
#endif

#if NO_STD

// --------------------------------------------------
//; 三角函数

#  if __has(sin)
static __attr(const) f32 sinf(f32 x) {
  return __builtin_sinf(x);
}
static __attr(const) f64 sin(f64 x) {
  return __builtin_sin(x);
}
#  else
static __attr(const) f32 sinf(f32 x) {
  x         = fmodf(x, (f32)(2 * PI));
  f32  sum  = x;
  f32  term = x;
  int  n    = 1;
  bool sign = true;
  while (term > F32_EPSILON || term < -F32_EPSILON) {
    n    += 2;
    term *= x * x / (n * (n - 1));
    sum  += sign ? -term : term;
    sign  = !sign;
  }
  return sum;
}
static __attr(const) f64 sin(f64 x) {
  x         = fmod(x, 2 * PI);
  f64  sum  = x;
  f64  term = x;
  int  n    = 1;
  bool sign = true;
  while (term > F64_EPSILON || term < -F64_EPSILON) {
    n    += 2;
    term *= x * x / (n * (n - 1));
    sum  += sign ? -term : term;
    sign  = !sign;
  }
  return sum;
}
#  endif

#  if __has(cos)
static __attr(const) f32 cosf(f32 x) {
  return __builtin_cosf(x);
}
static __attr(const) f64 cos(f64 x) {
  return __builtin_cos(x);
}
#  else
static __attr(const) f32 cosf(f32 x) {
  x         = fmodf(x, (f32)(2 * PI));
  f32  sum  = 1;
  f32  term = 1;
  int  n    = 0;
  bool sign = true;
  while (term > F32_EPSILON || term < -F32_EPSILON) {
    n    += 2;
    term *= x * x / (n * (n - 1));
    sum  += sign ? -term : term;
    sign  = !sign;
  }
  return sum;
}
static __attr(const) f64 cos(f64 x) {
  x         = fmod(x, 2 * PI);
  f64  sum  = 1;
  f64  term = 1;
  int  n    = 0;
  bool sign = true;
  while (term > F64_EPSILON || term < -F64_EPSILON) {
    n    += 2;
    term *= x * x / (n * (n - 1));
    sum  += sign ? -term : term;
    sign  = !sign;
  }
  return sum;
}
#  endif

#  if __has(sincos)
static __attr(const) void sincosf(f32 x, f32 *s, f32 *c) {
  __builtin_sincosf(x, s, c);
}
static __attr(const) void sincos(f64 x, f64 *s, f64 *c) {
  __builtin_sincos(x, s, c);
}
#  else
static __attr(const) void sincosf(f32 x, f32 *s, f32 *c) {
  *s = sinf(x);
  *c = cosf(x);
}
static __attr(const) void sincos(f64 x, f64 *s, f64 *c) {
  *s = sin(x);
  *c = cos(x);
}
#  endif

#  if __has(tan)
static __attr(const) f32 tanf(f32 x) {
  return __builtin_tanf(x);
}
static __attr(const) f64 tan(f64 x) {
  return __builtin_tan(x);
}
#  else
static __attr(const) f32 tanf(f32 x) {
  return sinf(x) / cosf(x);
}
static __attr(const) f64 tan(f64 x) {
  return sin(x) / cos(x);
}
#  endif

#  if __has(asin)
static __attr(const) f32 asinf(f32 x) {
  return __builtin_asinf(x);
}
static __attr(const) f64 asin(f64 x) {
  return __builtin_asin(x);
}
#  else
static __attr(const) f32 asinf(f32 x) {
  f32 sum  = x;
  f32 term = x;
  int n    = 1;
  while (term > F32_EPSILON || term < -F32_EPSILON) {
    term *= (x * x * (2 * n - 1) * (2 * n - 1)) / (2 * n * (2 * n + 1));
    sum  += term;
    n++;
  }
  return sum;
}
static __attr(const) f64 asin(f64 x) {
  f64 sum  = x;
  f64 term = x;
  int n    = 1;
  while (term > F64_EPSILON || term < -F64_EPSILON) {
    term *= (x * x * (2 * n - 1) * (2 * n - 1)) / (2 * n * (2 * n + 1));
    sum  += term;
    n++;
  }
  return sum;
}
#  endif

#  if __has(acos)
static __attr(const) f32 acosf(f32 x) {
  return __builtin_acosf(x);
}
static __attr(const) f64 acos(f64 x) {
  return __builtin_acos(x);
}
#  else
static __attr(const) f32 acosf(f32 x) {
  return (f32)(PI / 2) - asinf(x);
}
static __attr(const) f64 acos(f64 x) {
  return PI / 2 - asin(x);
}
#  endif

#  if __has(atan)
static __attr(const) f32 atanf(f32 x) {
  return __builtin_atanf(x);
}
static __attr(const) f64 atan(f64 x) {
  return __builtin_atan(x);
}
#  else
static __attr(const) f32 atanf(f32 x) {
  f32  sum  = x;
  f32  term = x;
  int  n    = 1;
  bool sign = true;
  while (term > F32_EPSILON || term < -F32_EPSILON) {
    term *= x * x * (2 * n - 1) / (2 * n + 1);
    sum  += sign ? -term : term;
    sign  = !sign;
    n++;
  }
  return sum;
}
static __attr(const) f64 atan(f64 x) {
  f64  sum  = x;
  f64  term = x;
  int  n    = 1;
  bool sign = true;
  while (term > F64_EPSILON || term < -F64_EPSILON) {
    term *= x * x * (2 * n - 1) / (2 * n + 1);
    sum  += sign ? -term : term;
    sign  = !sign;
    n++;
  }
  return sum;
}
#  endif

#  if __has(atan2)
static __attr(const) f32 atan2f(f32 x, f32 y) {
  return __builtin_atan2f(x, y);
}
static __attr(const) f64 atan2(f64 x, f64 y) {
  return __builtin_atan2(x, y);
}
#  else
static __attr(const) f32 atan2f(f32 y, f32 x) {
  if (x > 0) return atanf(y / x);
  if (x < 0 && y >= 0) return atanf(y / x) + (f32)PI;
  if (x < 0 && y < 0) return atanf(y / x) - (f32)PI;
  if (x == 0 && y > 0) return (f32)(PI / 2);
  if (x == 0 && y < 0) return -(f32)(PI / 2);
  return 0;
}
static __attr(const) f64 atan2(f64 y, f64 x) {
  if (x > 0) return atan(y / x);
  if (x < 0 && y >= 0) return atan(y / x) + PI;
  if (x < 0 && y < 0) return atan(y / x) - PI;
  if (x == 0 && y > 0) return PI / 2;
  if (x == 0 && y < 0) return -PI / 2;
  return 0;
}
#  endif

#endif
