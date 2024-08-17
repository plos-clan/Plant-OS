#include <misc.h>
#if defined(__clang__) && defined(__x86_64__) && !NO_EXTFLOAT
#  define FFT(_x_) fftl##_x_
typedef f128  FT;
typedef cf128 CT;
#  include "impl.h"
#endif