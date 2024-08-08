#pragma once
#include <define.h>

#define INFINITY (__builtin_inff())
#define INF      (__builtin_inff())
#define NAN      (__builtin_nanf(""))

finline double isfinite(double v) {
  return __builtin_isfinite(v);
}

#define isnan(v) __builtin_isnan(v)
#define isinf(v) __builtin_isinf_sign(v)
