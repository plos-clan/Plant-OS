#include <misc.h>

static void _volume_fine_tuning2(f32 *data, size_t len, size_t k) {
  for (size_t i = k; i < len - k; i++) {
    bool s = data[i] < 0;
    f32  x = __builtin_fabsf(data[i]);
    f32  p = __builtin_fabsf(data[i - k]);
    f32  n = __builtin_fabsf(data[i + k]);
    if (p != 0 && x > p * 4) {
      x           += p;
      data[i - k]  = 0;
    }
    if (n != 0 && x > n * 4) {
      x           += n;
      data[i + k]  = 0;
    }
    data[i] = s ? -x : x;
  }
}

static void _volume_fine_tuning1(f32 *data, size_t len, f32 vol) {
  for (size_t i = 0; i < len; i++) {
    bool s = data[i] < 0;
    f32  x = __builtin_fabsf(data[i]);
    if (x * 96 < vol) data[i] = 0;
  }
}

static void volume_fine_tuning(f32 *data, size_t len, f32 *vol_p) {
#define vol (*vol_p)
  f32 max = 0;
  for (size_t i = 0; i < len; i++) {
    f32 x = __builtin_fabsf(data[i]);
    if (x > max) max = x;
  }
  vol = max >= vol ? max : vol * .9375 + max * .0625;
  _volume_fine_tuning1(data, len, vol);
  _volume_fine_tuning2(data, len, 1);
  _volume_fine_tuning2(data, len, 2);
#undef vol
}
