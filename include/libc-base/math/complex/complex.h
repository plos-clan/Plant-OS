#pragma once
#include "../exp-log.h"
#include "../sin.h"
#include <define.h>

#if NO_STD

#  if __has(cexp)
inline_const cf32 cexpf(cf32 z) {
  return __builtin_cexpf(z);
}
inline_const cf64 cexp(cf64 z) {
  return __builtin_cexp(z);
}
#  else
static cf32 cexpf(cf32 z) {
  double exp_real = expf(__real__ z);
  cf32   real     = exp_real * cos(__imag__ z);
  cf32   imag     = exp_real * sin(__imag__ z);
  return real + imag * 1.fi;
}
static cf64 cexp(cf64 z) {
  double exp_real = exp(__real__ z);
  f64    real     = exp_real * cos(__imag__ z);
  f64    imag     = exp_real * sin(__imag__ z);
  return real + imag * 1.i;
}
#  endif

#endif
