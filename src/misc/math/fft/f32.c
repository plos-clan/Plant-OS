#include <misc.h>

#define bit_rev(n) (bit_reverse((u64)(n)) >> (64 - log_n))

//* float32

static cf32 fftf_wn[64] = {};
static cf32 aft_wn[64]  = {};
static bool fftf_inited = false;

static void fftf_init() {
  if (atom_tas(&fftf_inited)) return;
  f32 n = 1;
  for (int i = 0; i < 64; i++) {
    cf32 x      = 6.283185307179586232i * n;
    fftf_wn[i]  = __builtin_cexp(x);
    aft_wn[i]   = __builtin_cexp(-x);
    n          *= .5;
  }
}

// y 输出
// s 输入
// l 输入长度
// p 对齐大小 (p 个采样)
// r 是否为逆变换

dlexport void fftf_p(cf32 *y, cf32 *s, size_t l, size_t p, bool r) {
  if (!fftf_inited) fftf_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf32 *x = malloc(n * sizeof(cf32));

  for (int i = 0; i < l; i++)
    x[bit_rev(i)] = s[i * p];
  for (int i = l; i < n; i++)
    x[bit_rev(i)] = 0;

  cf32 *_wn = (r ? aft_wn : fftf_wn);
  cf32  w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = _wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf32 t            = w * x[k + j + m / 2];
        cf32 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    f32 d = 1. / n;
    for (int i = 0; i < l; i++)
      y[i * p] = x[i] * d;
  } else {
    for (int i = 0; i < l; i++)
      y[i * p] = x[i];
  }

  free(x);
}

dlexport void fftf(cf32 *x, cf32 *s, size_t l, bool r) {
  if (!fftf_inited) fftf_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  for (int i = 0; i < l; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l; i < n; i++)
    x[bit_rev(i)] = 0;

  cf32 *_wn = (r ? aft_wn : fftf_wn);
  cf32  w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = _wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf32 t            = w * x[k + j + m / 2];
        cf32 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    f32 d = 1. / n;
    for (int i = 0; i < l; i++)
      x[i] *= d;
  }
}

dlexport cf32 *fftf_a(cf32 *s, size_t l, bool r) {
  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf32 *x = malloc(n * sizeof(cf32));

  fftf(x, s, l, r);

  return x;
}

dlexport void fftf_r2r_p(f32 *y, f32 *s, size_t l, size_t p, bool r) {
  if (!fftf_inited) fftf_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf32 *x = malloc(n * sizeof(cf32));

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

  cf32 *_wn = (r ? aft_wn : fftf_wn);
  cf32  w, wn;

  for (int s = 1; s <= log_n; s++) {
    size_t m = 1 << s;
    wn       = _wn[s];
    for (size_t k = 0; k < n; k += m) {
      w = 1;
      for (size_t j = 0; j < m / 2; j++) {
        cf32 t            = w * x[k + j + m / 2];
        cf32 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    y[0 * p] = __real__ x[0] / n;
    y[1 * p] = __real__ x[l / 2] / n;
    f32 d    = 2. / n;
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

dlexport void fftf_r2r(f32 *y, f32 *s, size_t l, bool r) {
  if (!fftf_inited) fftf_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf32 *x = malloc(n * sizeof(cf32));

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

  cf32 *_wn = (r ? aft_wn : fftf_wn);
  cf32  w, wn;

  for (int s = 1; s <= log_n; s++) {
    size_t m = 1 << s;
    wn       = _wn[s];
    for (size_t k = 0; k < n; k += m) {
      w = 1;
      for (size_t j = 0; j < m / 2; j++) {
        cf32 t            = w * x[k + j + m / 2];
        cf32 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    y[0]  = __real__ x[0] / n;
    y[1]  = __real__ x[l / 2] / n;
    f32 d = 2. / n;
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

dlexport f32 *fftf_r2r_a(f32 *s, size_t l, bool r) {
  if (!fftf_inited) fftf_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf32 *x = malloc(n * sizeof(cf32));

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

  cf32 *_wn = (r ? aft_wn : fftf_wn);
  cf32  w, wn;

  for (int s = 1; s <= log_n; s++) {
    size_t m = 1 << s;
    wn       = _wn[s];
    for (size_t k = 0; k < n; k += m) {
      w = 1;
      for (size_t j = 0; j < m / 2; j++) {
        cf32 t            = w * x[k + j + m / 2];
        cf32 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  if (!r) {
    ((f32 *)x)[0] = __real__ x[0] / n;
    ((f32 *)x)[1] = __real__ x[l / 2] / n;
    f32 d         = 2. / n;
    for (int i = 1; i < l / 2; i++)
      x[i] *= d;
  } else {
    for (int i = 0; i < l; i++)
      ((f32 *)x)[i] = __real__ x[i];
  }

  return (f32 *)x;
  // return realloc(x, n * sizeof(f32));
}

dlexport void fftf_r2c(cf32 *x, f32 *s, size_t l) {
  if (!fftf_inited) fftf_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  for (int i = 0; i < l; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l; i < n; i++)
    x[bit_rev(i)] = 0;

  cf32 w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = fftf_wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf32 t            = w * x[k + j + m / 2];
        cf32 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  x[0]  /= n;
  f32 d  = 2. / n;
  for (int i = 1; i < l / 2; i++)
    x[i] *= d;
  x[l / 2] /= n;
}

dlexport cf32 *fftf_r2c_a(f32 *s, size_t l) {
  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf32 *x = malloc(n * sizeof(cf32));

  fftf_r2c(x, s, l);

  return x;
}

dlexport void fftf_c2r(f32 *y, cf32 *s, size_t l) {
  if (!fftf_inited) fftf_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf32 *x = malloc(n * sizeof(cf32));

  for (int i = 0; i <= l / 2; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l / 2 + 1; i < n; i++)
    x[bit_rev(i)] = 0;

  cf32 w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = aft_wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf32 t            = w * x[k + j + m / 2];
        cf32 u            = x[k + j];
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

dlexport f32 *fftf_c2r_a(cf32 *s, size_t l) {
  if (!fftf_inited) fftf_init();

  int    log_n = 64 - clz(l) - !(l & (l - 1));
  size_t n     = 1 << log_n;

  cf32 *x = malloc(n * sizeof(cf32));

  for (int i = 0; i <= l / 2; i++)
    x[bit_rev(i)] = s[i];
  for (int i = l / 2 + 1; i < n; i++)
    x[bit_rev(i)] = 0;

  cf32 w, wn;

  for (int s = 1; s <= log_n; s++) {
    int m = 1 << s;
    wn    = aft_wn[s];
    for (int k = 0; k < n; k += m) {
      w = 1;
      for (int j = 0; j < m / 2; j++) {
        cf32 t            = w * x[k + j + m / 2];
        cf32 u            = x[k + j];
        x[k + j]          = u + t;
        x[k + j + m / 2]  = u - t;
        w                *= wn;
      }
    }
  }

  f32 *y = (void *)x;
  for (int i = 1; i < l; i++) {
    y[i] = __real__ x[i];
  }

  return y;
}

#undef bit_rev

dlexport void fftf_2d_p(cf32 *x, cf32 *s, size_t w, size_t h, size_t p, bool r) {
  for (size_t i = 0; i < h; i++) {
    fftf_p(x + p * i, s + p * i, w, 1, r);
  }
  for (size_t i = 0; i < w; i++) {
    fftf_p(x + i, x + i, h, p, r);
  }
}

dlexport void fftf_2d(cf32 *x, cf32 *s, size_t w, size_t h, bool r) {
  fftf_2d_p(x, s, w, h, w, r);
}

dlexport cf32 *fftf_2d_ap(cf32 *s, size_t w, size_t h, size_t p, bool r) {
  cf32 *x = malloc(p * h * sizeof(cf32));
  fftf_2d_p(x, s, w, h, p, r);
  return x;
}

dlexport cf32 *fftf_2d_a(cf32 *s, size_t w, size_t h, bool r) {
  cf32 *x = malloc(w * h * sizeof(cf32));
  fftf_2d(x, s, w, h, r);
  return x;
}

dlexport void fftf_2d_r2r_p(f32 *x, f32 *s, size_t w, size_t h, size_t p, bool r) {
  for (size_t i = 0; i < h; i++) {
    fftf_r2r_p(x + p * i, s + p * i, w, 1, r);
  }
  for (size_t i = 0; i < w; i++) {
    fftf_r2r_p(x + i, x + i, h, p, r);
  }
}

dlexport void fftf_2d_r2r(f32 *x, f32 *s, size_t w, size_t h, bool r) {
  fftf_2d_r2r_p(x, s, w, h, w, r);
}

dlexport f32 *fftf_2d_r2r_ap(f32 *s, size_t w, size_t h, size_t p, bool r) {
  f32 *x = malloc(w * h * sizeof(f32));
  fftf_2d_r2r_p(x, s, w, h, p, r);
  return x;
}

dlexport f32 *fftf_2d_r2r_a(f32 *s, size_t w, size_t h, bool r) {
  f32 *x = malloc(w * h * sizeof(f32));
  fftf_2d_r2r(x, s, w, h, r);
  return x;
}
