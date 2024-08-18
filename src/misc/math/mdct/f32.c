#include <misc.h>
#define FFT(_x_)  fftf##_x_
#define MDCT(_x_) mdctf##_x_
typedef f32  FT;
typedef cf32 CT;
#include "impl.h"