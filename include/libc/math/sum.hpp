#pragma once
#include "base.hpp"
#include <define.h>
#include <type.hpp>

namespace cpp {

// --------------------------------------------------
//; 求和

template <typename T>
static auto sum(T x) -> T {
  return x;
}
template <typename T, typename... Args>
static auto sum(T x, Args... args) -> T {
  T y = sum(args...);
  return x + y;
}

template <typename T>
static auto sumi(T x) -> i32 {
  return x;
}
template <typename T, typename... Args>
static auto sumi(T x, Args... args) -> i32 {
  i32 y = sumi(args...);
  return x + y;
}

template <typename T>
static auto suml(T x) -> i64 {
  return x;
}
template <typename T, typename... Args>
static auto suml(T x, Args... args) -> i64 {
  i64 y = sumi(args...);
  return x + y;
}

template <typename T>
static auto sumf(T x) -> f32 {
  return x;
}
template <typename T, typename... Args>
static auto sumf(T x, Args... args) -> f32 {
  f32 y = sumf(args...);
  return x + y;
}

template <typename T>
static auto sumd(T x) -> f64 {
  return x;
}
template <typename T, typename... Args>
static auto sumd(T x, Args... args) -> f64 {
  f64 y = sumd(args...);
  return x + y;
}

// --------------------------------------------------
//; 平均数

template <typename T, typename... Args>
static auto avg(T x, Args... args) -> T {
  return sum(args...) / sizeof...(args);
}

template <typename T, typename... Args>
static auto avgf(T x, Args... args) -> f32 {
  return sumf(args...) / (f32)sizeof...(args);
}

template <typename T, typename... Args>
static auto avgd(T x, Args... args) -> f64 {
  return sumd(args...) / (f64)sizeof...(args);
}

} // namespace cpp
