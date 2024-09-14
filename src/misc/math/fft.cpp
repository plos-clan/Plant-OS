#include <misc.h>

namespace cpp {

void fft(cf64 *x, cf64 *s, size_t l, size_t p, bool r) {
  ::fft_p(x, s, l, p, r);
}
void fft(cf64 *x, cf64 *s, size_t l, bool r) {
  ::fft(x, s, l, r);
}
auto fft(cf64 *s, size_t l, bool r) -> cf64 * {
  return ::fft_a(s, l, r);
}
void fft(cf32 *x, cf32 *s, size_t l, size_t p, bool r) {
  ::fftf_p(x, s, l, p, r);
}
void fft(cf32 *x, cf32 *s, size_t l, bool r) {
  ::fftf(x, s, l, r);
}
auto fft(cf32 *s, size_t l, bool r) -> cf32 * {
  return ::fftf_a(s, l, r);
}

void fft_r2r(f64 *x, f64 *s, size_t l, size_t p, bool r) {
  ::fft_r2r_p(x, s, l, p, r);
}
void fft_r2r(f64 *x, f64 *s, size_t l, bool r) {
  ::fft_r2r(x, s, l, r);
}
auto fft_r2r(f64 *s, size_t l, bool r) -> f64 * {
  return ::fft_r2r_a(s, l, r);
}
void fft_r2r(f32 *x, f32 *s, size_t l, size_t p, bool r) {
  ::fftf_r2r_p(x, s, l, p, r);
}
void fft_r2r(f32 *x, f32 *s, size_t l, bool r) {
  ::fftf_r2r(x, s, l, r);
}
auto fft_r2r(f32 *s, size_t l, bool r) -> f32 * {
  return ::fftf_r2r_a(s, l, r);
}

auto fft_r2c(f64 *s, size_t l) -> cf64 * {
  return ::fft_r2c_a(s, l);
}
auto fft_r2c(f32 *s, size_t l) -> cf32 * {
  return ::fftf_r2c_a(s, l);
}

auto fft_c2r(cf64 *s, size_t l) -> f64 * {
  return ::fft_c2r_a(s, l);
}
auto fft_c2r(cf32 *s, size_t l) -> f32 * {
  return ::fftf_c2r_a(s, l);
}

void fft_2d(cf64 *x, cf64 *s, size_t w, size_t h, size_t p, bool r) {
  ::fft_2d_p(x, s, w, h, p, r);
}
void fft_2d(cf64 *x, cf64 *s, size_t w, size_t h, bool r) {
  ::fft_2d(x, s, w, h, r);
}
auto fft_2d(cf64 *s, size_t w, size_t h, bool r) -> cf64 * {
  return ::fft_2d_a(s, w, h, r);
}
void fft_2d(cf32 *x, cf32 *s, size_t w, size_t h, size_t p, bool r) {
  ::fftf_2d_p(x, s, w, h, p, r);
}
void fft_2d(cf32 *x, cf32 *s, size_t w, size_t h, bool r) {
  ::fftf_2d(x, s, w, h, r);
}
auto fft_2d(cf32 *s, size_t w, size_t h, bool r) -> cf32 * {
  return ::fftf_2d_a(s, w, h, r);
}

void fft_2d_r2r(f64 *x, f64 *s, size_t w, size_t h, size_t p, bool r) {
  ::fft_2d_r2r_p(x, s, w, h, p, r);
}
void fft_2d_r2r(f64 *x, f64 *s, size_t w, size_t h, bool r) {
  ::fft_2d_r2r(x, s, w, h, r);
}
auto fft_2d_r2r(f64 *s, size_t w, size_t h, bool r) -> f64 * {
  return ::fft_2d_r2r_a(s, w, h, r);
}
void fft_2d_r2r(f32 *x, f32 *s, size_t w, size_t h, size_t p, bool r) {
  ::fftf_2d_r2r_p(x, s, w, h, p, r);
}
void fft_2d_r2r(f32 *x, f32 *s, size_t w, size_t h, bool r) {
  ::fftf_2d_r2r(x, s, w, h, r);
}
auto fft_2d_r2r(f32 *s, size_t w, size_t h, bool r) -> f32 * {
  return ::fftf_2d_r2r_a(s, w, h, r);
}

} // namespace cpp
