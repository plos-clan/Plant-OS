// This code is released under the MIT License

// author: copi143

#include <libc-base.h>

#if 0
static double erf(double x) {
  const double a1 = 0.254829592;
  const double a2 = -0.284496736;
  const double a3 = 1.421413741;
  const double a4 = -1.453152027;
  const double a5 = 1.061405429;
  const double p  = 0.3275911;

  bool sign = x < 0;
  if (sign) x = -x;
  double t = 1.0 / (1.0 + p * x);
  double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);
  return sign ? -y : y;
}

static double erfinv(double x) {
  const double a = 0.147;

  double ln1    = log(1.0 - x * x);
  double ln2    = log(1.0 + x * x);
  double part1  = 2.0 / (PI * a) + ln1 / 2.0;
  double part2  = ln1 / a;
  double result = sqrt(sqrt(part1 * part1 - part2) - part1);
  return x < 0 ? -result : result;
}

static f32 uniform_to_normal(f32 x) {
  return 1.41421356237309504880168872420969807 * erfinv(2 * x - 1);
}

static f32 normal_to_uniform(f32 x) {
  return .5 * (1 + erf(x / 1.41421356237309504880168872420969807));
}
#endif

#define MU 1023

static void mulaw_compress(f32 *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    f32  x    = data[i];
    bool sign = x < 0;
    if (sign) x = -x;
    x       = __builtin_log(1 + MU * x) / __builtin_log(1 + MU);
    data[i] = sign ? -x : x;
  }
}

static void mulaw_expand(f32 *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    f32  x    = data[i];
    bool sign = x < 0;
    if (sign) x = -x;
    x       = (__builtin_pow(1 + MU, x) - 1) / MU;
    data[i] = sign ? -x : x;
  }
}

#undef MU
