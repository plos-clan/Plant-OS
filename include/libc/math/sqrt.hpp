#pragma once
#include <define.h>
#include <type.hpp>

namespace cpp {

// --------------------------------------------------
//; 平方 立方 平方根 立方根

template <typename T>
static auto square(T x) -> T {
  return x * x;
}

template <typename T>
static auto cube(T x) -> T {
  return x * x * x;
}

#if __has(sqrt)
finline auto sqrt(float x) -> float {
  return __builtin_sqrtf(x);
}
finline auto sqrt(double x) -> double {
  return __builtin_sqrt(x);
}
finline auto sqrt(i32 x) -> float {
  return sqrt((float)x);
}
finline auto sqrt(u32 x) -> float {
  return sqrt((float)x);
}
finline auto sqrt(i64 x) -> double {
  return sqrt((double)x);
}
finline auto sqrt(u64 x) -> double {
  return sqrt((double)x);
}
#else
static auto sqrt(float x) -> float {
  x           = x < 0 ? -x : x;
  float guess = .6;
  guess       = (guess + x / guess) / 2;
  guess       = (guess + x / guess) / 2;
  guess       = (guess + x / guess) / 2;
  guess       = (guess + x / guess) / 2;
  guess       = (guess + x / guess) / 2;
  return guess;
}
static auto sqrt(double x) -> double {
  x            = x < 0 ? -x : x;
  double guess = .6;
  guess        = (guess + x / guess) / 2;
  guess        = (guess + x / guess) / 2;
  guess        = (guess + x / guess) / 2;
  guess        = (guess + x / guess) / 2;
  guess        = (guess + x / guess) / 2;
  return guess;
}
finline auto sqrt(i32 x) -> float {
  return sqrt((float)x);
}
finline auto sqrt(u32 x) -> float {
  return sqrt((float)x);
}
finline auto sqrt(i64 x) -> double {
  return sqrt((double)x);
}
finline auto sqrt(u64 x) -> double {
  return sqrt((double)x);
}
#endif

#if __has(cbrt)
finline auto cbrt(float x) -> float {
  return __builtin_cbrtf(x);
}
finline auto cbrt(double x) -> double {
  return __builtin_cbrt(x);
}
#else
static auto cbrt(float x) -> float {
  bool neg = x < 0;
  if (neg) x = -x;
  float guess = .6;
  guess       = (2 * guess + x / (guess * guess)) / 3;
  guess       = (2 * guess + x / (guess * guess)) / 3;
  guess       = (2 * guess + x / (guess * guess)) / 3;
  guess       = (2 * guess + x / (guess * guess)) / 3;
  guess       = (2 * guess + x / (guess * guess)) / 3;
  return neg ? -guess : guess;
}
static auto cbrt(double x) -> double {
  bool neg = x < 0;
  if (neg) x = -x;
  double guess = .6;
  guess        = (2 * guess + x / (guess * guess)) / 3;
  guess        = (2 * guess + x / (guess * guess)) / 3;
  guess        = (2 * guess + x / (guess * guess)) / 3;
  guess        = (2 * guess + x / (guess * guess)) / 3;
  guess        = (2 * guess + x / (guess * guess)) / 3;
  return neg ? -guess : guess;
}
#endif

} // namespace cpp
