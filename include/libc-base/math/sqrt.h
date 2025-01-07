#pragma once
#include <define.h>

//* ----------------------------------------------------------------------------------------------------
//; 标准库的
//* ----------------------------------------------------------------------------------------------------

#if NO_STD

// --------------------------------------------------
//; 平方根 立方根

#  if __has(sqrt) && !defined(_SQRT_C_)
inline_const float sqrtf(float x) {
  return __builtin_sqrtf(x);
}
inline_const double sqrt(double x) {
  return __builtin_sqrt(x);
}
#  else
dlimport float  sqrtf(float x);
dlimport double sqrt(double x);
#  endif

#  if __has(cbrt) && !defined(_SQRT_C_)
inline_const float cbrtf(float x) {
  return __builtin_cbrtf(x);
}
inline_const double cbrt(double x) {
  return __builtin_cbrt(x);
}
#  else
dlimport float  cbrtf(float x);
dlimport double cbrt(double x);
#  endif

#endif

#if __has(sqrt) && !defined(_SQRT_C_)
inline_const float fourth_rootf(float x) {
  return __builtin_sqrtf(__builtin_sqrtf(x));
}
inline_const double fourth_root(double x) {
  return __builtin_sqrt(__builtin_sqrt(x));
}
#else
dlimport float  fourth_rootf(float x);
dlimport double fourth_root(double x);
#endif

#if __has(pow) && !defined(_SQRT_C_)
inline_const float fifth_rootf(float x) {
  return __builtin_powf(x, 1.0f / 5.0f);
}
inline_const double fifth_root(double x) {
  return __builtin_pow(x, 1.0 / 5.0);
}
#else
dlimport float  fifth_rootf(float x);
dlimport double fifth_root(double x);
#endif
