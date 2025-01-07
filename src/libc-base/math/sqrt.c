#define _SQRT_C_
#include <libc-base.h>

//* ----------------------------------------------------------------------------------------------------
//; 标准库的
//* ----------------------------------------------------------------------------------------------------

#if NO_STD

// --------------------------------------------------
//; 平方根 立方根

dlexport constfn float sqrtf(float x) {
  x           = x < 0 ? -x : x;
  float guess = .6;
  guess       = (guess + x / guess) / 2;
  guess       = (guess + x / guess) / 2;
  guess       = (guess + x / guess) / 2;
  guess       = (guess + x / guess) / 2;
  guess       = (guess + x / guess) / 2;
  return guess;
}

dlexport constfn double sqrt(double x) {
  x            = x < 0 ? -x : x;
  double guess = .6;
  guess        = (guess + x / guess) / 2;
  guess        = (guess + x / guess) / 2;
  guess        = (guess + x / guess) / 2;
  guess        = (guess + x / guess) / 2;
  guess        = (guess + x / guess) / 2;
  return guess;
}

dlexport constfn float cbrtf(float x) {
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

dlexport constfn double cbrt(double x) {
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

dlexport constfn float fourth_rootf(float x) {
  return sqrtf(sqrtf(x));
}
dlexport constfn double fourth_root(double x) {
  return sqrt(sqrt(x));
}

dlexport constfn float fifth_rootf(float x) {
  return powf(x, 1.0f / 5.0f);
}
dlexport constfn double fifth_root(double x) {
  return pow(x, 1.0 / 5.0);
}

#endif
