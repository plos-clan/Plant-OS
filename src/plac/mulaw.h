#include <libc-base.h>

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
