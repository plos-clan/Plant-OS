#pragma once
#include <define.h>
#include <type.h>

#define INFINITY (__builtin_inff())
#define INF      (__builtin_inff())
#define NAN      (__builtin_nanf(""))

finline double isfinite(double v) {
  return __builtin_isfinite(v);
}
