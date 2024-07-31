#pragma once
#include <libc-base.h>

namespace cpp {

dlimport void fft(cf64 *x, cf64 *s, size_t l, size_t p, bool r);
dlimport void fft(cf64 *x, cf64 *s, size_t l, bool r);
dlimport auto fft(cf64 *s, size_t l, bool r) -> cf64 *;
dlimport void fft(cf32 *x, cf32 *s, size_t l, size_t p, bool r);
dlimport void fft(cf32 *x, cf32 *s, size_t l, bool r);
dlimport auto fft(cf32 *s, size_t l, bool r) -> cf32 *;

dlimport void fft_r2r(f64 *x, f64 *s, size_t l, size_t p, bool r);
dlimport void fft_r2r(f64 *x, f64 *s, size_t l, bool r);
dlimport auto fft_r2r(f64 *s, size_t l, bool r) -> f64 *;
dlimport void fft_r2r(f32 *x, f32 *s, size_t l, size_t p, bool r);
dlimport void fft_r2r(f32 *x, f32 *s, size_t l, bool r);
dlimport auto fft_r2r(f32 *s, size_t l, bool r) -> f32 *;

dlimport auto fft_r2c(f64 *s, size_t l) -> cf64 *;
dlimport auto fft_r2c(f32 *s, size_t l) -> cf32 *;

dlimport auto fft_c2r(cf64 *s, size_t l) -> f64 *;
dlimport auto fft_c2r(cf32 *s, size_t l) -> f32 *;

dlimport void fft_2d(cf64 *x, cf64 *s, size_t w, size_t h, size_t p, bool r);
dlimport void fft_2d(cf64 *x, cf64 *s, size_t w, size_t h, bool r);
dlimport auto fft_2d(cf64 *s, size_t w, size_t h, bool r) -> cf64 *;
dlimport void fft_2d(cf32 *x, cf32 *s, size_t w, size_t h, size_t p, bool r);
dlimport void fft_2d(cf32 *x, cf32 *s, size_t w, size_t h, bool r);
dlimport auto fft_2d(cf32 *s, size_t w, size_t h, bool r) -> cf32 *;

dlimport void fft_2d_r2r(f64 *x, f64 *s, size_t w, size_t h, size_t p, bool r);
dlimport void fft_2d_r2r(f64 *x, f64 *s, size_t w, size_t h, bool r);
dlimport auto fft_2d_r2r(f64 *s, size_t w, size_t h, bool r) -> f64 *;
dlimport void fft_2d_r2r(f32 *x, f32 *s, size_t w, size_t h, size_t p, bool r);
dlimport void fft_2d_r2r(f32 *x, f32 *s, size_t w, size_t h, bool r);
dlimport auto fft_2d_r2r(f32 *s, size_t w, size_t h, bool r) -> f32 *;

} // namespace cpp
