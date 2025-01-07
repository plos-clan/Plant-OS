#pragma once
#include <define.h>

//* ----------------------------------------------------------------------------------------------------
//; 标准库的
//* ----------------------------------------------------------------------------------------------------

#if NO_STD

// --------------------------------------------------
//; 对数 指数

#  if __has(exp2) && !defined(_EXP_LOG_C_)
inline_const f32 exp2f(f32 x) {
  return __builtin_exp2f(x);
}
inline_const f64 exp2(f64 x) {
  return __builtin_exp2(x);
}
#  else
dlimport __attr(const) f32 exp2f(f32 x);
dlimport __attr(const) f64 exp2(f64 x);
#  endif

#  if __has(log2) && !defined(_EXP_LOG_C_)
inline_const f32 log2f(f32 x) {
  return __builtin_log2f(x);
}
inline_const f64 log2(f64 x) {
  return __builtin_log2(x);
}
#  else
dlimport __attr(const) f32 log2f(f32 x);
dlimport __attr(const) f64 log2(f64 x);
#  endif

#  if __has(exp) && !defined(_EXP_LOG_C_)
inline_const f32 expf(f32 x) {
  return __builtin_expf(x);
}
inline_const f64 exp(f64 x) {
  return __builtin_exp(x);
}
#  else
dlimport __attr(const) f32 expf(f32 x);
dlimport __attr(const) f64 exp(f64 x);
#  endif

#  if __has(log) && !defined(_EXP_LOG_C_)
inline_const f32 logf(f32 x) {
  return __builtin_logf(x);
}
inline_const f64 log(double x) {
  return __builtin_log(x);
}
#  else
dlimport __attr(const) float  logf(float x);
dlimport __attr(const) double log(double x);
#  endif

inline_const f32 powfu(f32 a, u32 b) {
  f32 r = 1;
  while (b > 0) {
    if (b & 1) r *= a;
    a  *= a;
    b >>= 1;
  }
  return r;
}

inline_const f32 powfi(f32 a, i32 b) {
  return b < 0 ? 1 / powfu(a, -b) : powfu(a, b);
}

inline_const i32 powi32(i32 a, u32 b) {
  i32 r = 1;
  while (b > 0) {
    if (b & 1) r *= a;
    a  *= a;
    b >>= 1;
  }
  return r;
}

inline_const f64 powu(f64 a, u64 b) {
  f64 r = 1;
  while (b > 0) {
    if (b & 1) r *= a;
    a  *= a;
    b >>= 1;
  }
  return r;
}

inline_const f64 powi(f64 a, i64 b) {
  return b < 0 ? 1 / powu(a, -b) : powu(a, b);
}

inline_const i64 powi64(i64 a, u64 b) {
  i64 r = 1;
  while (b > 0) {
    if (b & 1) r *= a;
    a  *= a;
    b >>= 1;
  }
  return r;
}

#  if __has(pow) && !defined(_EXP_LOG_C_)
inline_const f32 powf(f32 a, f32 b) {
  return __builtin_powf(a, b);
}
inline_const f64 pow(f64 a, f64 b) {
  return __builtin_pow(a, b);
}
#  else
dlimport __attr(const) f32 powf(f32 a, f32 b);
dlimport __attr(const) f64 pow(f64 a, f64 b);
#  endif

#endif
