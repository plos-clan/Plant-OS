#include <misc.h>

#define bit_rev(n) (bit_reverse((u64)(n)) >> (64 - log_n))

//* float64

static cf64 fft_wn[64] = {};
static cf64 aft_wn[64] = {};
static bool fft_inited = false;

static void fft_init() {
  if (atom_tas(&fft_inited)) return;
  f64 n = 1;
  for (int i = 0; i < 64; i++) {
    cf64 x     = 6.283185307179586232i * n;
    fft_wn[i]  = __builtin_cexp(x);
    aft_wn[i]  = __builtin_cexp(-x);
    n         *= .5;
  }
}

// y 输出
// s 输入
// l 输入长度
// p 对齐大小 (p 个采样)
// r 是否为逆变换

dlexport void fft_p(cf64 *y, cf64 *s, size_t l, size_t p, bool r) {
  if (!fft_inited) fft_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf64 *x = malloc(n * sizeof(cf64));

  for (int i = 0; i < l; i++)
    x[bit_rev(i)] = s[i * p];
  for (int i = l; i < n; i++)
    x[bit_rev(i)] = 0;

  cf64 *_wn = (r ? aft_wn : fft_wn);
  cf64  w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = _wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf64 t            = w * x[k + j + m / 2];
        cf64 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    f64 d = 1. / n;
    for (int i = 0; i < l; i++)
      y[i * p] = x[i] * d;
  } else {
    for (int i = 0; i < l; i++)
      y[i * p] = x[i];
  }

  free(x);
}

dlexport void fft(cf64 *x, cf64 *s, size_t l, bool r) {
  if (!fft_inited) fft_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  for (int i = 0; i < l; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l; i < n; i++)
    x[bit_rev(i)] = 0;

  cf64 *_wn = (r ? aft_wn : fft_wn);
  cf64  w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = _wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf64 t            = w * x[k + j + m / 2];
        cf64 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    f64 d = 1. / n;
    for (int i = 0; i < l; i++)
      x[i] *= d;
  }
}

dlexport cf64 *fft_a(cf64 *s, size_t l, bool r) {
  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf64 *x = malloc(n * sizeof(cf64));

  fft(x, s, l, r);

  return x;
}

dlexport void fft_r2r_p(f64 *y, f64 *s, size_t l, size_t p, bool r) {
  if (!fft_inited) fft_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf64 *x = malloc(n * sizeof(cf64));

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

  cf64 *_wn = (r ? aft_wn : fft_wn);
  cf64  w, wn;

  for (int s = 1; s <= log_n; s++) {
    size_t m = 1 << s;
    wn       = _wn[s];
    for (size_t k = 0; k < n; k += m) {
      w = 1;
      for (size_t j = 0; j < m / 2; j++) {
        cf64 t            = w * x[k + j + m / 2];
        cf64 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    y[0 * p] = __real__ x[0] / n;
    y[1 * p] = __real__ x[l / 2] / n;
    f64 d    = 2. / n;
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

dlexport void fft_r2r(f64 *y, f64 *s, size_t l, bool r) {
  if (!fft_inited) fft_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf64 *x = malloc(n * sizeof(cf64));

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

  cf64 *_wn = (r ? aft_wn : fft_wn);
  cf64  w, wn;

  for (int s = 1; s <= log_n; s++) {
    size_t m = 1 << s;
    wn       = _wn[s];
    for (size_t k = 0; k < n; k += m) {
      w = 1;
      for (size_t j = 0; j < m / 2; j++) {
        cf64 t            = w * x[k + j + m / 2];
        cf64 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    y[0]  = __real__ x[0] / n;
    y[1]  = __real__ x[l / 2] / n;
    f64 d = 2. / n;
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

dlexport f64 *fft_r2r_a(f64 *s, size_t l, bool r) {
  if (!fft_inited) fft_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf64 *x = malloc(n * sizeof(cf64));

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

  cf64 *_wn = (r ? aft_wn : fft_wn);
  cf64  w, wn;

  for (int s = 1; s <= log_n; s++) {
    size_t m = 1 << s;
    wn       = _wn[s];
    for (size_t k = 0; k < n; k += m) {
      w = 1;
      for (size_t j = 0; j < m / 2; j++) {
        cf64 t            = w * x[k + j + m / 2];
        cf64 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    ((f64 *)x)[0] = __real__ x[0] / n;
    ((f64 *)x)[1] = __real__ x[l / 2] / n;
    f64 d         = 2. / n;
    for (int i = 1; i < l / 2; i++)
      x[i] *= d;
  } else {
    for (int i = 0; i < l; i++)
      ((f64 *)x)[i] = __real__ x[i];
  }

  return (f64 *)x;
  // return realloc(x, n * sizeof(f64));
}

dlexport void fft_r2c(cf64 *x, f64 *s, size_t l) {
  if (!fft_inited) fft_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  for (int i = 0; i < l; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l; i < n; i++)
    x[bit_rev(i)] = 0;

  cf64 w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = fft_wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf64 t            = w * x[k + j + m / 2];
        cf64 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  x[0]  /= n;
  f64 d  = 2. / n;
  for (int i = 1; i < l / 2; i++)
    x[i] *= d;
  x[l / 2] /= n;
}

dlexport cf64 *fft_r2c_a(f64 *s, size_t l) {
  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf64 *x = malloc(n * sizeof(cf64));

  fft_r2c(x, s, l);

  return x;
}

dlexport void fft_c2r(f64 *y, cf64 *s, size_t l) {
  if (!fft_inited) fft_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf64 *x = malloc(n * sizeof(cf64));

  for (int i = 0; i <= l / 2; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l / 2 + 1; i < n; i++)
    x[bit_rev(i)] = 0;

  cf64 w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = aft_wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf64 t            = w * x[k + j + m / 2];
        cf64 u            = x[k + j];
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

dlexport f64 *fft_c2r_a(cf64 *s, size_t l) {
  if (!fft_inited) fft_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf64 *x = malloc(n * sizeof(cf64));

  for (int i = 0; i <= l / 2; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l / 2 + 1; i < n; i++)
    x[bit_rev(i)] = 0;

  cf64 w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = aft_wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf64 t            = w * x[k + j + m / 2];
        cf64 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  f64 *y = (void *)x;
  for (int i = 1; i < l; i++) {
    y[i] = __real__ x[i];
  }

  return y;
}

#undef bit_rev

dlexport void fft_2d_p(cf64 *x, cf64 *s, size_t w, size_t h, size_t p, bool r) {
  for (size_t i = 0; i < h; i++) {
    fft_p(x + p * i, s + p * i, w, 1, r);
  }
  for (size_t i = 0; i < w; i++) {
    fft_p(x + i, x + i, h, p, r);
  }
}

dlexport void fft_2d(cf64 *x, cf64 *s, size_t w, size_t h, bool r) {
  fft_2d_p(x, s, w, h, w, r);
}

dlexport cf64 *fft_2d_ap(cf64 *s, size_t w, size_t h, size_t p, bool r) {
  cf64 *x = malloc(p * h * sizeof(cf64));
  fft_2d_p(x, s, w, h, p, r);
  return x;
}

dlexport cf64 *fft_2d_a(cf64 *s, size_t w, size_t h, bool r) {
  cf64 *x = malloc(w * h * sizeof(cf64));
  fft_2d(x, s, w, h, r);
  return x;
}

dlexport void fft_2d_r2r_p(f64 *x, f64 *s, size_t w, size_t h, size_t p, bool r) {
  for (size_t i = 0; i < h; i++) {
    fft_r2r_p(x + p * i, s + p * i, w, 1, r);
  }
  for (size_t i = 0; i < w; i++) {
    fft_r2r_p(x + i, x + i, h, p, r);
  }
}

dlexport void fft_2d_r2r(f64 *x, f64 *s, size_t w, size_t h, bool r) {
  fft_2d_r2r_p(x, s, w, h, w, r);
}

dlexport f64 *fft_2d_r2r_ap(f64 *s, size_t w, size_t h, size_t p, bool r) {
  f64 *x = malloc(w * h * sizeof(f64));
  fft_2d_r2r_p(x, s, w, h, p, r);
  return x;
}

dlexport f64 *fft_2d_r2r_a(f64 *s, size_t w, size_t h, bool r) {
  f64 *x = malloc(w * h * sizeof(f64));
  fft_2d_r2r(x, s, w, h, r);
  return x;
}
