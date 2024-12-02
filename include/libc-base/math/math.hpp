#pragma once
#include "base.hpp"
#include <define.hpp>

namespace cpp {

// 计算两个数的差值（非负）
template <typename T>
inline_const auto diff(T x, T y) -> T {
  return x < y ? y - x : x - y;
}

template <typename T>
inline_const auto equals(const T &a, const T &b) -> bool {
  return a == b;
}
// 浮点相近就判断为相等
template <>
constfn equals(const f32 &a, const f32 &b)->bool {
  return a - b < F32_EPSILON && a - b > -F32_EPSILON;
}
// 浮点相近就判断为相等
template <>
constfn equals(const f64 &a, const f64 &b)->bool {
  return a - b < F64_EPSILON && a - b > -F64_EPSILON;
}

// --------------------------------------------------
//; 最大最小

template <typename T>
inline_const auto min(T x) -> T {
  return x;
}
template <typename T, typename... Args>
inline_const auto min(T x, Args... args) -> T {
  T y = min(args...);
  return x < y ? x : y;
}

template <typename T>
inline_const auto max(T x) -> T {
  return x;
}
template <typename T, typename... Args>
inline_const auto max(T x, Args... args) -> T {
  T y = max(args...);
  return x > y ? x : y;
}

// 将 v 限制到 [min,max] 的范围内
template <typename T>
inline_const auto clamp(const T &v, const T &min, const T &max) -> const T & {
  if (v < min) return min;
  if (v > max) return max;
  return v;
}

// --------------------------------------------------
//;

template <typename T>
inline_const auto abs(T x) -> T {
  return x >= 0 ? x : -x;
}
template <typename T>
inline_const auto abs(T x, T y) -> T {
  return sqrt(x * x + y * y);
}
template <typename T>
inline_const auto abs(T x, T y, T z) -> T {
  return sqrt(x * x + y * y + z * z);
}
template <typename T>
inline_const auto abs(T x, T y, T z, T w) -> T {
  return sqrt(x * x + y * y + z * z + w * w);
}

template <typename T>
inline_const auto gcd(T a, T b) -> T {
  while (b != 0) {
    T t = b;
    b   = a % b;
    a   = t;
  }
  return a;
}

template <typename T>
inline_const auto lcm(T a, T b) -> T {
  return a / gcd(a, b) * b;
}

#if __has(fmod)
inline_const auto mod(f32 x, f32 y) -> f32 {
  return __builtin_fmodf(x, y);
}
inline_const auto mod(f64 x, f64 y) -> f64 {
  return __builtin_fmod(x, y);
}
#else
inline_const auto mod(f32 x, f32 y) -> f32 {
  return x - (i32)(x / y) * y;
}
inline_const auto mod(f64 x, f64 y) -> f64 {
  return x - (i64)(x / y) * y;
}
#endif

inline_const auto factorial(u32 n) -> u64 {
  u64 result = 1;
  for (u32 i = 2; i <= n; i++) {
    result *= i;
  }
  return result;
}

} // namespace cpp
