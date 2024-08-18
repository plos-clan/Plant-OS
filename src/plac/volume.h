#include <misc.h>

void volume_fine_tuning(f32 *data, size_t len, f32 *vol) {
  f32 max = 0;
  for (size_t i = 0; i < len; i++) {
    f32 x = __builtin_fabsf(data[i]);
    if (x > max) max = x;
  }
  *vol = *vol * .9 + max * .1;
  if (__builtin_fabsf(data[0]) < .1 * *vol) data[0] = 0;
  for (size_t i = 1; i < len - 1; i++) {
    bool s = data[i] < 0;
    f32  x = __builtin_fabsf(data[i]);
    if (x < 0.01 * *vol) {
      data[i] = 0;
      continue;
    }
    f32 p = __builtin_fabsf(data[i - 1]);
    f32 n = __builtin_fabsf(data[i + 1]);
    if (x > p * 5) {
      x           += p;
      data[i - 1]  = 0;
    }
    if (x > n * 5) {
      x           += n;
      data[i + 1]  = 0;
    }
    data[i] = s ? -x : x;
  }
  if (__builtin_fabsf(data[len - 1]) < .1 * *vol) data[len - 1] = 0;
}
