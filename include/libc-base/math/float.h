#pragma once
#include <define.h>

#if NO_STD
#  define INFINITY (__builtin_inff())
#  define NAN      (__builtin_nanf(""))
#else
#  ifdef __cplusplus
#    include <cmath>
#  else
#    include <math.h>
#  endif
#endif
#define INF (__builtin_inff())

#if NO_STD
finline double isfinite(double v) {
  return __builtin_isfinite(v);
}
#endif

#if NO_STD
#  define isnan(v) __builtin_isnan(v)
#  define isinf(v) __builtin_isinf_sign(v)
#endif
