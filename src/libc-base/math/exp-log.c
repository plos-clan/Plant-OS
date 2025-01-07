#define _EXP_LOG_C_ 1
#include <libc-base.h>

//* ----------------------------------------------------------------------------------------------------
//; 标准库的
//* ----------------------------------------------------------------------------------------------------
// `` NO_STD ``

#if NO_STD

// --------------------------------------------------
//; 对数 指数

dlexport constfn f32 exp2f(f32 x) {
  f32 y;
  i32 e;
  e   = (i32)(x + 127);
  x  += 127 - e;
  e <<= 23;
  y   = *(f32 *)&e;
  x  *= x * .339766027f + .660233972f;
  return (x + 1) * y;
}

dlexport constfn f32 log2f(f32 x) {
  i32 y;
  f32 r;
  y   = *(i32 *)&x;
  y >>= 23;
  r   = (f32)y;
  y   = *(i32 *)&x;
  y   = (y & 0x007fffff) | 0x3f800000;
  x   = *(f32 *)&y;
  r  += -128 + x * (x * -0.333333333f + 2) - 0.666666666f;
  return r;
}

dlexport constfn f64 exp2(f64 x) {
  f64 y;
  i64 e;
  e   = (i64)(x + 1023);
  x  += 1023 - e;
  e <<= 52;
  y   = *(f64 *)&e;
  x  *= x * .339766027 + .660233972;
  return (x + 1) * y;
}

dlexport constfn f64 log2(f64 x) {
  i64 y;
  f64 r;
  y   = *(i64 *)&x;
  y >>= 52;
  r   = (f64)y;
  y   = *(i64 *)&x;
  y   = (y & 0x000fffffffffffff) | 0x3ff0000000000000;
  x   = *(f64 *)&y;
  r  += -1024 + x * (x * -0.3333333333333333 + 2) - 0.6666666666666666;
  return r;
}

dlexport constfn float expf(float x) {
  return exp2f(x * 1.442695040888963f);
}

dlexport constfn double exp(double x) {
  return exp2(x * 1.442695040888963);
}

dlexport constfn f32 powf(f32 a, f32 b) {
  i32 c  = b;
  b     -= c;
  return exp2(b * log2(a)) * powfi(a, c);
}
dlexport constfn f64 pow(f64 a, f64 b) {
  i64 c  = b;
  b     -= c;
  return exp2(b * log2(a)) * powi(a, c);
}

dlexport constfn float logf(float x) {
  return log2f(x) * 0.693147180559945f;
}

dlexport constfn double log(double x) {
  return log2(x) * 0.693147180559945;
}

#endif
