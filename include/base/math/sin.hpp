#pragma once
#include "base.hpp"
#include "math.hpp"
#include <define.h>
#include <type.hpp>

namespace cpp {

// --------------------------------------------------
//; 三角函数

#if __has(sin)
static auto sin(f32 x) -> f32 {
  return __builtin_sinf(x);
}
static auto sin(f64 x) -> f64 {
  return __builtin_sin(x);
}
#else
static auto sin(f32 x) -> f32 {
  x         = mod(x, (f32)(2 * PI));
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
static auto sin(f64 x) -> f64 {
  x         = mod(x, 2 * PI);
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
#endif

#if __has(cos)
static auto cos(f32 x) -> f32 {
  return __builtin_cosf(x);
}
static auto cos(f64 x) -> f64 {
  return __builtin_cos(x);
}
#else
static auto cos(f32 x) -> f32 {
  x         = mod(x, (f32)(2 * PI));
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
static auto cos(f64 x) -> f64 {
  x         = mod(x, 2 * PI);
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
#endif

#if __has(tan)
static auto tan(f32 x) -> f32 {
  return __builtin_tanf(x);
}
static auto tan(f64 x) -> f64 {
  return __builtin_tan(x);
}
#else
static auto tan(f32 x) -> f32 {
  return sin(x) / cos(x);
}
static auto tan(f64 x) -> f64 {
  return sin(x) / cos(x);
}
#endif

#if __has(asin)
static auto asin(f32 x) -> f32 {
  return __builtin_asinf(x);
}
static auto asin(f64 x) -> f64 {
  return __builtin_asin(x);
}
#else
static auto asin(f32 x) -> f32 {
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
static auto asin(f64 x) -> f64 {
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
#endif

#if __has(acos)
static auto acos(f32 x) -> f32 {
  return __builtin_acosf(x);
}
static auto acos(f64 x) -> f64 {
  return __builtin_acos(x);
}
#else
static auto acos(f32 x) -> f32 {
  return (f32)(PI / 2) - asin(x);
}
static auto acos(f64 x) -> f64 {
  return PI / 2 - asin(x);
}
#endif

#if __has(atan)
static auto atan(f32 x) -> f32 {
  return __builtin_atanf(x);
}
static auto atan(f64 x) -> f64 {
  return __builtin_atan(x);
}
#else
static auto atan(f32 x) -> f32 {
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
static auto atan(f64 x) -> f64 {
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
#endif

#if __has(atan2)
static auto atan2(f32 x, f32 y) -> f32 {
  return __builtin_atan2f(x, y);
}
static auto atan2(f64 x, f64 y) -> f64 {
  return __builtin_atan2(x, y);
}
#else
static auto atan2(f32 y, f32 x) -> f32 {
  if (x > 0) return atan(y / x);
  if (x < 0 && y >= 0) return atan(y / x) + (f32)PI;
  if (x < 0 && y < 0) return atan(y / x) - (f32)PI;
  if (x == 0 && y > 0) return (f32)(PI / 2);
  if (x == 0 && y < 0) return -(f32)(PI / 2);
  return 0;
}
static auto atan2(f64 y, f64 x) -> f64 {
  if (x > 0) return atan(y / x);
  if (x < 0 && y >= 0) return atan(y / x) + PI;
  if (x < 0 && y < 0) return atan(y / x) - PI;
  if (x == 0 && y > 0) return PI / 2;
  if (x == 0 && y < 0) return -PI / 2;
  return 0;
}
#endif

static auto atan(f32 x, f32 y) -> f32 {
  return atan2(x, y);
}
static auto atan(f64 x, f64 y) -> f64 {
  return atan2(x, y);
}

} // namespace cpp
