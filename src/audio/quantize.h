// This code is released under the MIT License

// author: copi143

#include <audio.h>

static void quantize(quantized_t q) {
  if (q->nbit == 0) goto zero;
  f32 max = q->dataf[0];
  f32 min = q->dataf[0];
  for (size_t i = 1; i < q->len; i++) {
    if (q->dataf[i] > max) max = q->dataf[i];
    if (q->dataf[i] < min) min = q->dataf[i];
  }
  if (-1e-6 < max - min && max - min < 1e-6) goto zero;
  q->max      = __builtin_ceil(max * 256);
  q->min      = __builtin_floor(min * 256);
  max         = (f32)q->max / 256;
  min         = (f32)q->min / 256;
  const f32 k = ((1 << q->nbit) - 1) / (max - min);
  q->mid      = (i16)(-min * k);
  for (size_t i = 0; i < q->len; i++) {
    q->datai[i] = (i16)((q->dataf[i] - min) * k);
  }
  return;

zero:
  q->mid = 0;
  for (size_t i = 0; i < q->len; i++) {
    q->datai[i] = 0;
  }
}

static void dequantize(quantized_t q) {
  f32 max = (f32)q->max / 256;
  f32 min = (f32)q->min / 256;
  if (q->nbit == 0) {
    for (size_t i = 0; i < q->len; i++) {
      q->dataf[i] = min;
    }
    return;
  }
  f32 k = (max - min) / ((1 << q->nbit) - 1);
  for (size_t i = 0; i < q->len; i++) {
    q->dataf[i] = q->datai[i] * k + min;
  }
}

static f32 quantize_diff(quantized_t q) {
  f32 max = (f32)q->max / 256;
  f32 min = (f32)q->min / 256;
  if (q->nbit == 0) {
    f32 diff = 0;
    for (size_t i = 0; i < q->len; i++) {
      diff += (q->dataf[i] - min) * (q->dataf[i] - min);
    }
    return diff / q->len;
  }
  f32       diff = 0;
  const f32 k    = (max - min) / ((1 << q->nbit) - 1);
  for (size_t i = 0; i < q->len; i++) {
    f32 d  = q->datai[i] * k + min;
    diff  += (q->dataf[i] - d) * (q->dataf[i] - d);
  }
  return diff / q->len;
}

static void best_quantize(quantized_t q, size_t from, size_t to, f32 target) {
  for (size_t nbit = from; nbit <= to; nbit++) {
    q->nbit = nbit;
    quantize(q);
    if (quantize_diff(q) < target) break;
  }
}

static f32 diff(f32 *a, f32 *b, size_t len) {
  f32 d = 0;
  for (size_t i = 0; i < len; i++) {
    d += (a[i] - b[i]) * (a[i] - b[i]);
  }
  return d / len;
}
