#include <misc.h>
#define FFT(_x_)  fft##_x_
#define MDCT(_x_) mdct##_x_
typedef f64  FT;
typedef cf64 CT;
#include "impl.h"