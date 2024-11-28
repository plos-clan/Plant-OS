#pragma once
#include <libc-base.h>

#define _FFT_(FFT, FT, CT)                                                                         \
  dlimport void FFT##_p(CT *x, CT *s, size_t l, size_t p, bool r);                                 \
  dlimport void FFT(CT *x, CT *s, size_t l, bool r);                                               \
  dlimport CT  *FFT##_a(CT *s, size_t l, bool r);                                                  \
  dlimport void FFT##_r2r_p(FT *x, FT *s, size_t l, size_t p, bool r);                             \
  dlimport void FFT##_r2r(FT *x, FT *s, size_t l, bool r);                                         \
  dlimport FT  *FFT##_r2r_a(FT *s, size_t l, bool r);                                              \
  dlimport void FFT##_r2c(CT *x, FT *s, size_t l);                                                 \
  dlimport CT  *FFT##_r2c_a(FT *s, size_t l);                                                      \
  dlimport void FFT##_c2r(FT *x, CT *s, size_t l);                                                 \
  dlimport FT  *FFT##_c2r_a(CT *s, size_t l);

_FFT_(fft, f64, cf64);
_FFT_(fftf, f32, cf32);
#if defined(__clang__) && defined(__x86_64__) && !NO_EXTFLOAT
_FFT_(fftl, f128, cf128);
#endif

#undef _FFT_

#define _FFT_2D_(FFT, FT, CT)                                                                      \
  dlimport void FFT##_2d_p(CT *x, CT *s, size_t w, size_t h, size_t p, bool r);                    \
  dlimport void FFT##_2d(CT *x, CT *s, size_t w, size_t h, bool r);                                \
  dlimport CT  *FFT##_2d_ap(CT *s, size_t w, size_t h, size_t p, bool r);                          \
  dlimport CT  *FFT##_2d_a(CT *s, size_t w, size_t h, bool r);                                     \
  dlimport void FFT##_2d_r2r_p(FT *x, FT *s, size_t w, size_t h, size_t p, bool r);                \
  dlimport void FFT##_2d_r2r(FT *x, FT *s, size_t w, size_t h, bool r);                            \
  dlimport FT  *FFT##_2d_r2r_ap(FT *s, size_t w, size_t h, size_t p, bool r);                      \
  dlimport FT  *FFT##_2d_r2r_a(FT *s, size_t w, size_t h, bool r);

_FFT_2D_(fft, f64, cf64);
_FFT_2D_(fftf, f32, cf32);
#if defined(__clang__) && defined(__x86_64__) && !NO_EXTFLOAT
_FFT_2D_(fftl, f128, cf128);
#endif

#undef _FFT_2D_
