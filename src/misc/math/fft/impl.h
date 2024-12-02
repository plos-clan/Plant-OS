#include <misc.h>

// 快速傅里叶变换

// FT: 浮点类型
// CT: 复数类型

#ifndef FFT
#  define FFT(_x_) fft##_x_
typedef f64  FT;
typedef cf64 CT;
#endif

#define bit_rev(n) (bit_reverse((u64)(n)) >> (64 - log_n))

static CT   fft_wn[64] = {};
static CT   aft_wn[64] = {};
static bool fft_inited = false;

#if __has(cexp)
#  define cexp(x) __builtin_cexp(x)
#else
inline_const f64 _fmod(f64 x, f64 y) {
  return x - (i32)(x / y) * y;
}
inline_const f64 _exp(f64 x) {
  f64 sum  = 1;
  f64 term = 1;
  for (int i = 1; term > F64_EPSILON; i++) {
    term *= x / i;
    sum  += term;
  }
  return sum;
}
inline_const f64 _sin(f64 x) {
  x         = _fmod(x, 2 * PI);
  f64  sum  = x;
  f64  term = x;
  int  n    = 1;
  bool sign = true;
  while (term > F64_EPSILON || term < -F64_EPSILON) {
    n    += 2;
    term *= x * x / (n * (n - 1));
    sum  += sign ? -term : term;
    sign  = !sign;
  }
  return sum;
}
inline_const f64 _cos(f64 x) {
  x         = _fmod(x, 2 * PI);
  f64  sum  = 1;
  f64  term = 1;
  int  n    = 0;
  bool sign = true;
  while (term > F64_EPSILON || term < -F64_EPSILON) {
    n    += 2;
    term *= x * x / (n * (n - 1));
    sum  += sign ? -term : term;
    sign  = !sign;
  }
  return sum;
}
inline_const cf64 _cexp(cf64 z) {
  double exp_real = _exp(__real__ z);
  f64    real     = exp_real * _cos(__imag__ z);
  f64    imag     = exp_real * _sin(__imag__ z);
  return real + imag * 1.i;
}
#  define cexp(x) _cexp(x)
#endif

static void FFT(_init)() {
  if (atom_tas(&fft_inited)) return;
  f64 n = 1;
  for (int i = 0; i < 64; i++) {
    cf64 x     = 6.283185307179586232i * n;
    fft_wn[i]  = cexp(x);
    aft_wn[i]  = cexp(-x);
    n         *= .5;
  }
}

#undef cexp

// y 输出
// x 输出
// s 输入
// l 输入长度
// p 步长
// r 是否为逆变换

dlexport void FFT(_p)(CT *y, CT *s, size_t l, size_t p, bool r) {
  if (!fft_inited) FFT(_init)();

  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  CT *x = malloc(n * sizeof(CT));

  for (int i = 0; i < l; i++)
    x[bit_rev(i)] = s[i * p];
  for (int i = l; i < n; i++)
    x[bit_rev(i)] = 0;

  CT *_wn = (r ? aft_wn : fft_wn);
  CT  w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = _wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        CT t              = w * x[k + j + m / 2];
        CT u              = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    FT d = 1. / n;
    for (int i = 0; i < l; i++)
      y[i * p] = x[i] * d;
  } else {
    for (int i = 0; i < l; i++)
      y[i * p] = x[i];
  }

  free(x);
}

dlexport void FFT()(CT *x, CT *s, size_t l, bool r) {
  if (!fft_inited) FFT(_init)();

  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  for (int i = 0; i < l; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l; i < n; i++)
    x[bit_rev(i)] = 0;

  CT *_wn = (r ? aft_wn : fft_wn);
  CT  w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = _wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        CT t              = w * x[k + j + m / 2];
        CT u              = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    FT d = 1. / n;
    for (int i = 0; i < l; i++)
      x[i] *= d;
  }
}

dlexport CT *FFT(_a)(CT *s, size_t l, bool r) {
  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  CT *x = malloc(n * sizeof(CT));

  FFT()(x, s, l, r);

  return x;
}

dlexport void FFT(_r2r_p)(FT *y, FT *s, size_t l, size_t p, bool r) {
  if (!fft_inited) FFT(_init)();

  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  CT *x = malloc(n * sizeof(CT));

  if (!r) {
    for (int i = 0; i < l; i++)
      x[bit_rev(i)] = s[i * p];
    for (int i = l; i < n; i++)
      x[bit_rev(i)] = 0;
  } else {
    x[0]              = s[0 * p];
    x[bit_rev(l / 2)] = s[1 * p];
    for (int i = 1; i < l / 2; i++)
      x[bit_rev(i)] = s[(i * 2) * p] + s[(i * 2 + 1) * p] * I;
    for (int i = l / 2 + 1; i < n; i++)
      x[bit_rev(i)] = 0;
  }

  CT *_wn = (r ? aft_wn : fft_wn);
  CT  w, wn;

  for (int s = 1; s <= log_n; s++) {
    size_t m = 1 << s;
    wn       = _wn[s];
    for (size_t k = 0; k < n; k += m) {
      w = 1;
      for (size_t j = 0; j < m / 2; j++) {
        CT t              = w * x[k + j + m / 2];
        CT u              = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    y[0 * p] = __real__ x[0] / n;
    y[1 * p] = __real__ x[l / 2] / n;
    FT d     = 2. / n;
    for (int i = 1; i < l / 2; i++) {
      y[(i * 2) * p]     = __real__ x[i] * d;
      y[(i * 2 + 1) * p] = __imag__ x[i] * d;
    }
  } else {
    for (int i = 0; i < l; i++)
      y[i * p] = __real__ x[i];
  }

  free(x);
}

dlexport void FFT(_r2r)(FT *y, FT *s, size_t l, bool r) {
  if (!fft_inited) FFT(_init)();

  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  CT *x = malloc(n * sizeof(CT));

  if (!r) {
    for (int i = 0; i < l; i++)
      x[bit_rev(i)] = s[i];
    for (int i = l; i < n; i++)
      x[bit_rev(i)] = 0;
  } else {
    x[0]              = s[0];
    x[bit_rev(l / 2)] = s[1];
    for (int i = 1; i < l / 2; i++)
      x[bit_rev(i)] = s[i * 2] + s[i * 2 + 1] * I;
    for (int i = l / 2 + 1; i < n; i++)
      x[bit_rev(i)] = 0;
  }

  CT *_wn = (r ? aft_wn : fft_wn);
  CT  w, wn;

  for (int s = 1; s <= log_n; s++) {
    size_t m = 1 << s;
    wn       = _wn[s];
    for (size_t k = 0; k < n; k += m) {
      w = 1;
      for (size_t j = 0; j < m / 2; j++) {
        CT t              = w * x[k + j + m / 2];
        CT u              = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    y[0] = __real__ x[0] / n;
    y[1] = __real__ x[l / 2] / n;
    FT d = 2. / n;
    for (int i = 1; i < l / 2; i++) {
      y[i * 2]     = __real__ x[i] * d;
      y[i * 2 + 1] = __imag__ x[i] * d;
    }
  } else {
    for (int i = 0; i < l; i++)
      y[i] = __real__ x[i];
  }

  free(x);
}

dlexport FT *FFT(_r2r_a)(FT *s, size_t l, bool r) {
  if (!fft_inited) FFT(_init)();

  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  CT *x = malloc(n * sizeof(CT));

  if (!r) {
    for (int i = 0; i < l; i++)
      x[bit_rev(i)] = s[i];
    for (int i = l; i < n; i++)
      x[bit_rev(i)] = 0;
  } else {
    x[0]              = s[0];
    x[bit_rev(l / 2)] = s[1];
    for (int i = 1; i < l / 2; i++)
      x[bit_rev(i)] = s[i * 2] + s[i * 2 + 1] * I;
    for (int i = l / 2 + 1; i < n; i++)
      x[bit_rev(i)] = 0;
  }

  CT *_wn = (r ? aft_wn : fft_wn);
  CT  w, wn;

  for (int s = 1; s <= log_n; s++) {
    size_t m = 1 << s;
    wn       = _wn[s];
    for (size_t k = 0; k < n; k += m) {
      w = 1;
      for (size_t j = 0; j < m / 2; j++) {
        CT t              = w * x[k + j + m / 2];
        CT u              = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    ((FT *)x)[0] = __real__ x[0] / n;
    ((FT *)x)[1] = __real__ x[l / 2] / n;
    FT d         = 2. / n;
    for (int i = 1; i < l / 2; i++)
      x[i] *= d;
  } else {
    for (int i = 0; i < l; i++)
      ((FT *)x)[i] = __real__ x[i];
  }

  return realloc(x, n * sizeof(FT));
}

dlexport void FFT(_r2c)(CT *x, FT *s, size_t l) {
  if (!fft_inited) FFT(_init)();

  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  for (int i = 0; i < l; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l; i < n; i++)
    x[bit_rev(i)] = 0;

  CT w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = fft_wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        CT t              = w * x[k + j + m / 2];
        CT u              = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  x[0] /= n;
  FT d  = 2. / n;
  for (int i = 1; i < l / 2; i++)
    x[i] *= d;
  x[l / 2] /= n;
}

dlexport CT *FFT(_r2c_a)(FT *s, size_t l) {
  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  CT *x = malloc(n * sizeof(CT));

  FFT(_r2c)(x, s, l);

  return x;
}

dlexport void FFT(_c2r)(FT *y, CT *s, size_t l) {
  if (!fft_inited) FFT(_init)();

  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  CT *x = malloc(n * sizeof(CT));

  for (int i = 0; i <= l / 2; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l / 2 + 1; i < n; i++)
    x[bit_rev(i)] = 0;

  CT w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = aft_wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        CT t              = w * x[k + j + m / 2];
        CT u              = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  for (int i = 1; i < l; i++) {
    y[i] = __real__ x[i];
  }

  free(x);
}

dlexport FT *FFT(_c2r_a)(CT *s, size_t l) {
  if (!fft_inited) FFT(_init)();

  int    log_n = sizeof(size_t) * 8 - clz(l - 1);
  size_t n     = 1 << log_n;

  CT *x = malloc(n * sizeof(CT));

  for (int i = 0; i <= l / 2; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l / 2 + 1; i < n; i++)
    x[bit_rev(i)] = 0;

  CT w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = aft_wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        CT t              = w * x[k + j + m / 2];
        CT u              = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  FT *y = (void *)x;
  for (int i = 1; i < l; i++) {
    y[i] = __real__ x[i];
  }

  return y;
}

#undef bit_rev

dlexport void FFT(_2d_p)(CT *x, CT *s, size_t w, size_t h, size_t p, bool r) {
  for (size_t i = 0; i < h; i++) {
    FFT(_p)(x + p * i, s + p * i, w, 1, r);
  }
  for (size_t i = 0; i < w; i++) {
    FFT(_p)(x + i, x + i, h, p, r);
  }
}

dlexport void FFT(_2d)(CT *x, CT *s, size_t w, size_t h, bool r) {
  FFT(_2d_p)(x, s, w, h, w, r);
}

dlexport CT *FFT(_2d_ap)(CT *s, size_t w, size_t h, size_t p, bool r) {
  CT *x = malloc(p * h * sizeof(CT));
  FFT(_2d_p)(x, s, w, h, p, r);
  return x;
}

dlexport CT *FFT(_2d_a)(CT *s, size_t w, size_t h, bool r) {
  CT *x = malloc(w * h * sizeof(CT));
  FFT(_2d)(x, s, w, h, r);
  return x;
}

dlexport void FFT(_2d_r2r_p)(FT *x, FT *s, size_t w, size_t h, size_t p, bool r) {
  for (size_t i = 0; i < h; i++) {
    FFT(_r2r_p)(x + p * i, s + p * i, w, 1, r);
  }
  for (size_t i = 0; i < w; i++) {
    FFT(_r2r_p)(x + i, x + i, h, p, r);
  }
}

dlexport void FFT(_2d_r2r)(FT *x, FT *s, size_t w, size_t h, bool r) {
  FFT(_2d_r2r_p)(x, s, w, h, w, r);
}

dlexport FT *FFT(_2d_r2r_ap)(FT *s, size_t w, size_t h, size_t p, bool r) {
  FT *x = malloc(w * h * sizeof(FT));
  FFT(_2d_r2r_p)(x, s, w, h, p, r);
  return x;
}

dlexport FT *FFT(_2d_r2r_a)(FT *s, size_t w, size_t h, bool r) {
  FT *x = malloc(w * h * sizeof(FT));
  FFT(_2d_r2r)(x, s, w, h, r);
  return x;
}
