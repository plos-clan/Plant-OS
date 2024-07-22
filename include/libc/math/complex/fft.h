#include <define.h>
#include <osapi.h>
#include <type.h>

dlimport void  fft_p(cf64 *x, cf64 *s, size_t l, size_t p, bool r);
dlimport void  fft(cf64 *x, cf64 *s, size_t l, bool r);
dlimport cf64 *fft_a(cf64 *s, size_t l, bool r);
dlimport void  fft_r2r_p(f64 *x, f64 *s, size_t l, size_t p, bool r);
dlimport void  fft_r2r(f64 *x, f64 *s, size_t l, bool r);
dlimport f64  *fft_r2r_a(f64 *s, size_t l, bool r);
dlimport void  fft_r2c(cf64 *x, f64 *s, size_t l);
dlimport cf64 *fft_r2c_a(f64 *s, size_t l);
dlimport void  fft_c2r(f64 *x, cf64 *s, size_t l);
dlimport f64  *fft_c2r_a(cf64 *s, size_t l);

dlimport void  fft_2d_p(cf64 *x, cf64 *s, size_t w, size_t h, size_t p, bool r);
dlimport void  fft_2d(cf64 *x, cf64 *s, size_t w, size_t h, bool r);
dlimport cf64 *fft_2d_ap(cf64 *s, size_t w, size_t h, size_t p, bool r);
dlimport cf64 *fft_2d_a(cf64 *s, size_t w, size_t h, bool r);
dlimport void  fft_2d_r2r_p(f64 *x, f64 *s, size_t w, size_t h, size_t p, bool r);
dlimport void  fft_2d_r2r(f64 *x, f64 *s, size_t w, size_t h, bool r);
dlimport f64  *fft_2d_r2r_ap(f64 *s, size_t w, size_t h, size_t p, bool r);
dlimport f64  *fft_2d_r2r_a(f64 *s, size_t w, size_t h, bool r);

dlimport void  fftf_p(cf32 *x, cf32 *s, size_t l, size_t p, bool r);
dlimport void  fftf(cf32 *x, cf32 *s, size_t l, bool r);
dlimport cf32 *fftf_a(cf32 *s, size_t l, bool r);
dlimport void  fftf_r2r_p(f32 *x, f32 *s, size_t l, size_t p, bool r);
dlimport void  fftf_r2r(f32 *x, f32 *s, size_t l, bool r);
dlimport f32  *fftf_r2r_a(f32 *s, size_t l, bool r);
dlimport void  fftf_r2c(cf32 *x, f32 *s, size_t l);
dlimport cf32 *fftf_r2c_a(f32 *s, size_t l);
dlimport void  fftf_c2r(f32 *x, cf32 *s, size_t l);
dlimport f32  *fftf_c2r_a(cf32 *s, size_t l);

dlimport void  fftf_2d_p(cf32 *x, cf32 *s, size_t w, size_t h, size_t p, bool r);
dlimport void  fftf_2d(cf32 *x, cf32 *s, size_t w, size_t h, bool r);
dlimport cf32 *fftf_2d_ap(cf32 *s, size_t w, size_t h, size_t p, bool r);
dlimport cf32 *fftf_2d_a(cf32 *s, size_t w, size_t h, bool r);
dlimport void  fftf_2d_r2r_p(f32 *x, f32 *s, size_t w, size_t h, size_t p, bool r);
dlimport void  fftf_2d_r2r(f32 *x, f32 *s, size_t w, size_t h, bool r);
dlimport f32  *fftf_2d_r2r_ap(f32 *s, size_t w, size_t h, size_t p, bool r);
dlimport f32  *fftf_2d_r2r_a(f32 *s, size_t w, size_t h, bool r);

#if defined(__clang__) && defined(__x86_64__)
dlimport void   fftl_p(cf128 *x, cf128 *s, size_t l, size_t p, bool r);
dlimport void   fftl(cf128 *x, cf128 *s, size_t l, bool r);
dlimport cf128 *fftl_a(cf128 *s, size_t l, bool r);
dlimport void   fftl_r2r_p(f128 *x, f128 *s, size_t l, size_t p, bool r);
dlimport void   fftl_r2r(f128 *x, f128 *s, size_t l, bool r);
dlimport f128  *fftl_r2r_a(f128 *s, size_t l, bool r);
dlimport void   fftl_r2c(cf128 *x, f128 *s, size_t l);
dlimport cf128 *fftl_r2c_a(f128 *s, size_t l);
dlimport void   fftl_c2r(f128 *x, cf128 *s, size_t l);
dlimport f128  *fftl_c2r_a(cf128 *s, size_t l);

dlimport void   fftl_2d_p(cf128 *x, cf128 *s, size_t w, size_t h, size_t p, bool r);
dlimport void   fftl_2d(cf128 *x, cf128 *s, size_t w, size_t h, bool r);
dlimport cf128 *fftl_2d_ap(cf128 *s, size_t w, size_t h, size_t p, bool r);
dlimport cf128 *fftl_2d_a(cf128 *s, size_t w, size_t h, bool r);
dlimport void   fftl_2d_r2r_p(f128 *x, f128 *s, size_t w, size_t h, size_t p, bool r);
dlimport void   fftl_2d_r2r(f128 *x, f128 *s, size_t w, size_t h, bool r);
dlimport f128  *fftl_2d_r2r_ap(f128 *s, size_t w, size_t h, size_t p, bool r);
dlimport f128  *fftl_2d_r2r_a(f128 *s, size_t w, size_t h, bool r);
#endif
