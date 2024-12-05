#pragma once
#include <define.h>

#define INFINITY (__builtin_inff())
#define NAN      (__builtin_nanf(""))
#define INF      (__builtin_inff())

#if NO_STD
#  define isfinite(v) __builtin_isfinite(v)
#else
#  ifndef isfinite
dlimport double isfinite(double v);
#  endif
#endif

#if NO_STD
#  define isnan(v) __builtin_isnan(v)
#  define isinf(v) __builtin_isinf_sign(v)
#endif
