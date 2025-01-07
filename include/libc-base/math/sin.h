#pragma once
#include <define.h>

// `` NO_STD ``

#if NO_STD

// --------------------------------------------------
//; 三角函数

#  if __has(sin) && !defined(_SIN_C_)
inline_const f32 sinf(f32 x) {
  return __builtin_sinf(x);
}
inline_const f64 sin(f64 x) {
  return __builtin_sin(x);
}
#  else
dlimport f32 sinf(f32 x);
dlimport f64 sin(f64 x);
#  endif

#  if __has(cos) && !defined(_SIN_C_)
inline_const f32 cosf(f32 x) {
  return __builtin_cosf(x);
}
inline_const f64 cos(f64 x) {
  return __builtin_cos(x);
}
#  else
dlimport f32 cosf(f32 x);
dlimport f64 cos(f64 x);
#  endif

#  if __has(sincos) && !defined(_SIN_C_)
inline_const void sincosf(f32 x, f32 *s, f32 *c) {
  __builtin_sincosf(x, s, c);
}
inline_const void sincos(f64 x, f64 *s, f64 *c) {
  __builtin_sincos(x, s, c);
}
#  else
dlimport void sincosf(f32 x, f32 *s, f32 *c);
dlimport void sincos(f64 x, f64 *s, f64 *c);
#  endif

#  if __has(tan) && !defined(_SIN_C_)
inline_const f32 tanf(f32 x) {
  return __builtin_tanf(x);
}
inline_const f64 tan(f64 x) {
  return __builtin_tan(x);
}
#  else
dlimport f32 tanf(f32 x);
dlimport f64 tan(f64 x);
#  endif

#  if __has(asin) && !defined(_SIN_C_)
inline_const f32 asinf(f32 x) {
  return __builtin_asinf(x);
}
inline_const f64 asin(f64 x) {
  return __builtin_asin(x);
}
#  else
dlimport f32 asinf(f32 x);
dlimport f64 asin(f64 x);
#  endif

#  if __has(acos) && !defined(_SIN_C_)
inline_const f32 acosf(f32 x) {
  return __builtin_acosf(x);
}
inline_const f64 acos(f64 x) {
  return __builtin_acos(x);
}
#  else
dlimport f32 acosf(f32 x);
dlimport f64 acos(f64 x);
#  endif

#  if __has(atan) && !defined(_SIN_C_)
inline_const f32 atanf(f32 x) {
  return __builtin_atanf(x);
}
inline_const f64 atan(f64 x) {
  return __builtin_atan(x);
}
#  else
dlimport f32 atanf(f32 x);
dlimport f64 atan(f64 x);
#  endif

#  if __has(atan2) && !defined(_SIN_C_)
inline_const f32 atan2f(f32 x, f32 y) {
  return __builtin_atan2f(x, y);
}
inline_const f64 atan2(f64 x, f64 y) {
  return __builtin_atan2(x, y);
}
#  else
dlimport f32 atan2f(f32 y, f32 x);
dlimport f64 atan2(f64 y, f64 x);
#  endif

#endif
