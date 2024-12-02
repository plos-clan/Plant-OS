#include <misc.h>

// 修改型离散余弦变换

// FT: 浮点类型
// CT: 复数类型

#ifndef MDCT
#  define FFT(_x_)  fft##_x_
#  define MDCT(_x_) mdct##_x_
typedef f64  FT;
typedef cf64 CT;
#endif

dlexport FT *MDCT(_a)(FT *s, size_t N, bool r) {
  const FT freq   = 2 * PI / N;
  const FT cfreq  = __builtin_cos(freq);
  const FT sfreq  = __builtin_sin(freq);
  const FT cfreq8 = __builtin_cos(freq / 8);
  const FT sfreq8 = __builtin_sin(freq / 8);

  if (!r) {
    FT *x = malloc(N * sizeof(FT));
    for (size_t k = 0; k < N / 2; k++) {
      FT t         = PI * (k + 0.5) / N;
      FT S         = __builtin_sin(t);
      x[k]         = s[k] * S;
      x[N - 1 - k] = s[N - 1 - k] * S;
    }
    s = x;
  }

  CT *x = malloc(N / 4 * sizeof(CT));
  if (!r) {
    FT C = cfreq8, S = sfreq8;
    FT tempr, tempi;
    for (size_t i = 0; i < N / 4; i++) {
      size_t n = N / 2 - 1 - 2 * i;
      if (i < (N >> 3))
        tempr = s[N / 4 + n] + s[N + N / 4 - 1 - n];
      else
        tempr = s[N / 4 + n] - s[N / 4 - 1 - n];

      n = 2 * i;
      if (i < (N >> 3))
        tempi = s[N / 4 + n] - s[N / 4 - 1 - n];
      else
        tempi = s[N / 4 + n] + s[N + N / 4 - 1 - n];

      x[i] = (tempr * C + tempi * S) + (tempi * C - tempr * S) * I;

      const FT _C = C * cfreq - S * sfreq;
      const FT _S = S * cfreq + C * sfreq;
      C = _C, S = _S;
    }
  } else {
    FT C = cfreq8, S = sfreq8;
    for (size_t i = 0; i < N / 4; i++) {
      FT tempr = -s[2 * i];
      FT tempi = s[N / 2 - 1 - 2 * i];

      x[i] = (tempr * C - tempi * S) + (tempi * C + tempr * S) * I;

      const FT _C = C * cfreq - S * sfreq, _S = S * cfreq + C * sfreq;
      C = _C, S = _S;
    }
  }

  if (!r) free(s);

  CT *y = malloc(N / 4 * sizeof(CT));
  FFT()(y, x, N / 4, r);
  free(x);

  FT *z = malloc(N * sizeof(FT));
  if (!r) {
    FT C = cfreq8, S = sfreq8;
    for (size_t i = 0; i < N / 4; i++) {
      FT tempr = 2 * (__real__ y[i] * C + __imag__ y[i] * S);
      FT tempi = 2 * (__imag__ y[i] * C - __real__ y[i] * S);

      z[2 * i]             = -tempr;
      z[N / 2 - 1 - 2 * i] = tempi;

      z[N / 2 + 2 * i] = -tempi;
      z[N - 1 - 2 * i] = tempr;

      const FT _C = C * cfreq - S * sfreq;
      const FT _S = S * cfreq + C * sfreq;
      C = _C, S = _S;
    }
  } else {
    FT C = cfreq8, S = sfreq8;
    for (size_t i = 0; i < N / 4; i++) {
      FT tempr = 0.5 * (__real__ y[i] * C - __imag__ y[i] * S);
      FT tempi = 0.5 * (__imag__ y[i] * C + __real__ y[i] * S);

      z[N / 2 + N / 4 - 1 - 2 * i] = tempr;
      if (i < (N >> 3))
        z[N / 2 + N / 4 + 2 * i] = tempr;
      else
        z[2 * i - N / 4] = -tempr;

      z[N / 4 + 2 * i] = tempi;
      if (i < (N >> 3))
        z[N / 4 - 1 - 2 * i] = -tempi;
      else
        z[N / 4 + N - 1 - 2 * i] = tempi;

      const FT _C = C * cfreq - S * sfreq;
      const FT _S = S * cfreq + C * sfreq;
      C = _C, S = _S;
    }
  }
  free(y);

  if (r) {
    FT *x = malloc(N * sizeof(FT));
    for (size_t k = 0; k < N / 2; k++) {
      FT t         = PI * (k + 0.5) / N;
      FT S         = __builtin_sin(t);
      x[k]         = z[k] * S;
      x[N - 1 - k] = z[N - 1 - k] * S;
    }
    free(z);
    z = x;
  }

  return z;
}

dlexport MDCT(_t) MDCT(_alloc)(size_t length, bool inverse, void (*callback)(FT *, void *)) {
  MDCT(_t) mdct = malloc(sizeof(*mdct));
  if (mdct == NULL) return NULL;
  mdct->inverse  = inverse;
  mdct->len      = length;
  mdct->buf      = malloc(length * sizeof(FT));
  mdct->block    = inverse ? malloc(length / 2 * sizeof(FT)) : null;
  mdct->bufp     = 0;
  mdct->output   = null;
  mdct->callback = callback;
  mdct->userdata = null;
  if (!inverse) {
    for (size_t i = 0; i < mdct->len / 2; i++) {
      mdct->buf[i] = 0;
    }
    mdct->bufp = mdct->len / 2;
  }
  return mdct;
}

dlexport void MDCT(_free)(MDCT(_t) mdct) {
  free(mdct->buf);
  free(mdct->block);
  free(mdct->output);
  free(mdct);
}

static void MDCT(_do_mdct)(MDCT(_t) mdct) {
  for (size_t i = mdct->bufp; i < mdct->len; i++) {
    mdct->buf[i] = 0;
  }

  free(mdct->output);
  mdct->output = MDCT(_a)(mdct->buf, mdct->len, false);
  mdct->bufp   = mdct->len / 2;

  for (size_t i = 0; i < mdct->len / 2; i++) {
    mdct->buf[i] = mdct->buf[i + mdct->len / 2];
  }

  if (mdct->callback) mdct->callback(mdct->output, mdct->userdata);
}

static void MDCT(_do_imdct)(MDCT(_t) mdct) {
  for (size_t i = mdct->bufp; i < mdct->len / 2; i++) {
    mdct->buf[i] = 0;
  }

  bool first = mdct->output == null;
  if (!first) {
    for (size_t i = 0; i < mdct->len / 2; i++) {
      mdct->block[i] = mdct->output[i + mdct->len / 2];
    }
  }

  free(mdct->output);
  mdct->output = MDCT(_a)(mdct->buf, mdct->len, true);
  mdct->bufp   = 0;

  if (first) return;

  for (size_t i = 0; i < mdct->len / 2; i++) {
    mdct->block[i] += mdct->output[i];
  }

  if (mdct->callback) mdct->callback(mdct->block, mdct->userdata);
}

static void MDCT(_do)(MDCT(_t) mdct) {
  if (!mdct->inverse) {
    MDCT(_do_mdct)(mdct);
  } else {
    MDCT(_do_imdct)(mdct);
  }
}

dlexport void MDCT(_put)(MDCT(_t) mdct, FT *data, size_t length) {
  for (size_t i = 0; i < length; i++) {
    mdct->buf[mdct->bufp++] = data[i];
    if (mdct->bufp == (mdct->inverse ? mdct->len / 2 : mdct->len)) MDCT(_do)(mdct);
  }
}

dlexport void MDCT(_final)(MDCT(_t) mdct) {
  if (mdct->bufp > 0) MDCT(_do)(mdct);
  if (!mdct->inverse) MDCT(_do)(mdct);
}

dlexport FT *MDCT(_block)(MDCT(_t) mdct) {
  return mdct->block;
}
