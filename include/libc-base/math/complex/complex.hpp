#pragma once
#include "../exp-log.hpp"
#include "../sin.hpp"
#include <define.hpp>

namespace cpp {

#if __has(cexp)
static auto cexp(cf32 z) -> cf32 {
  return __builtin_cexpf(z);
}
static auto cexp(cf64 z) -> cf64 {
  return __builtin_cexp(z);
}
#else
static auto cexp(cf32 z) -> cf32 {
  double exp_real = exp(__real__ z);
  cf32   real     = exp_real * cos(__imag__ z);
  cf32   imag     = exp_real * sin(__imag__ z);
  return real + imag * 1.fi;
}
static auto cexp(cf64 z) -> cf64 {
  double exp_real = exp(__real__ z);
  f64    real     = exp_real * cos(__imag__ z);
  f64    imag     = exp_real * sin(__imag__ z);
  return real + imag * 1.i;
}
#endif

} // namespace cpp
