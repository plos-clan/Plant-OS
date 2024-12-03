#include <libc-base.h>

#if 0

#  ifdef __x86_64__

void mpi_set0(u64 *a, size_t len) {
  memset(a, 0, len * 8);
}

// a = b
void mpi_set(u64 *a, u64 *b, size_t len) {
  memcpy(a, b, len * 8);
}

// a += b
bool mpi_add2(u64 *a, u64 *b, size_t len) {
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_add_overflow(a[i], overflow, &a[i]);
    overflow |= __builtin_add_overflow(a[i], b[i], &a[i]);
  }
  return overflow;
}
// c = a + b
bool mpi_add3(u64 *c, u64 *a, u64 *b, size_t len) {
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_add_overflow(a[i], overflow, &c[i]);
    overflow |= __builtin_add_overflow(c[i], b[i], &c[i]);
  }
  return overflow;
}

// a -= b
bool mpi_sub2(u64 *a, u64 *b, size_t len) {
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_sub_overflow(a[i], overflow, &a[i]);
    overflow |= __builtin_sub_overflow(a[i], b[i], &a[i]);
  }
  return overflow;
}
// c = a - b
bool mpi_sub3(u64 *c, u64 *a, u64 *b, size_t len) {
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_sub_overflow(a[i], overflow, &c[i]);
    overflow |= __builtin_sub_overflow(c[i], b[i], &c[i]);
  }
  return overflow;
}

// a *= b
bool mpi_mul2(u64 *a, u64 *b, size_t len) {
  bool overflow = false;
  //   for (size_t i = 0; i < len; i++) {
  //     overflow = __builtin_add_overflow(a[i], overflow, &a[i]);
  //     overflow |= __builtin_add_overflow(a[i], b[i], &a[i]);
  //   }
  return overflow;
}

// a <<= b
void mpi_lshift2(u64 *a, size_t b, size_t len) {
  size_t _b  = b / 64;
  b         &= 63;
  for (size_t i = len - 1; i > _b; i--) {
    a[i] = (a[i - _b] << b) | (a[i - _b - 1] >> (64 - b));
  }
  a[_b] = a[0] << b;
  for (ssize_t i = _b - 1; i >= 0; i--)
    a[i] = 0;
}
// c = a << b
void mpi_lshift3(u64 *c, u64 *a, size_t b, size_t len) {
  size_t _b  = b / 64;
  b         &= 63;
  for (size_t i = 0; i < _b; i++)
    c[i] = 0;
  c[_b] = a[0] << b;
  for (size_t i = _b + 1; i < len; i++) {
    c[i] = (a[i - _b] << b) | (a[i - _b - 1] >> (64 - b));
  }
}

// a >>= b
u64 mpi_rshift2(u64 *a, byte b, size_t len) {
  u64 overflow = 0;
  for (ssize_t i = len - 1; i >= 0; i--) {
    u64 o    = a[i] << (64 - b);
    a[i]     = (a[i] >> b) | overflow;
    overflow = o;
  }
  return overflow;
}
// c = a >> b
u64 mpi_rshift3(u64 *c, u64 *a, byte b, size_t len) {
  u64 overflow = 0;
  for (ssize_t i = len - 1; i >= 0; i--) {
    c[i]     = (a[i] >> b) | overflow;
    overflow = a[i] << (64 - b);
  }
  return overflow;
}

// 异常返回 true
// 除数的内存地址不能与商或余数相同
// c = a / b  d = a % b
void mpi_div(u64 *c, u64 *d, u64 *a, u64 *b, size_t len) {
  u64 *_d = d;
  memcpy(d, a, len * 8);
  memset(c, 0, len * 8);
  a           = d;
  ssize_t hba = mpi_fhsb(a, len);
  ssize_t hbb = mpi_fhsb(b, len);
  if (hba < hbb) { return; }
  int h = hba - hbb;
  if (h) mpi_lshift2(b, h, len);
  d = alloca(len * 8);

  void *t;
  bool  _ = !mpi_sub3(d, a, b, len);
  if (_) (t = a, a = d, d = t, c[h / 64] |= (u64)1 << (h & 63));
  for (h--; h >= 0; h--) {
    mpi_rshift2(b, 1, len);
    _ = !mpi_sub3(d, a, b, len);
    if (_) (t = a, a = d, d = t, c[h / 64] |= (u64)1 << (h & 63));
  }

  if (a != _d) memcpy(_d, a, len * 8);
}

// 比较数字
int mpi_cmp(u64 *a, u64 *b, size_t len) {
  for (ssize_t i = len - 1; i >= 0; i--) {
    if (a[i] < b[i]) return -1;
    if (a[i] > b[i]) return 1;
  }
  return 0;
}

// a = -a
void mpi_neg(u64 *a, size_t len) {
  for (size_t i = 0; i < len; i++)
    a[i] = ~a[i];
  bool overflow = true;
  for (size_t i = 0; i < len && overflow; i++)
    overflow = __builtin_add_overflow(a[i], 1, &a[i]);
}
// b = -a
void mpi_neg2(u64 *b, u64 *a, size_t len) {
  for (size_t i = 0; i < len; i++)
    b[i] = ~a[i];
  bool overflow = true;
  for (size_t i = 0; i < len && overflow; i++)
    overflow = __builtin_add_overflow(b[i], 1, &b[i]);
}

// fhsb(a)
ssize_t mpi_fhsb(u64 *a, size_t len) {
  ssize_t n;
  for (ssize_t i = len - 1; i; i--)
    if ((n = fhsb64(a[i])) >= 0) return n + i * 64;
  return fhsb64(a[0]);
}

extern const char _plos_lut_alnum_lower[62];
extern const char _plos_lut_alnum_upper[62];

char *mpi_tostr(char *buf, size_t buflen, u64 *_n, size_t len, int b, bool _signed) {
  cstr nlt = _plos_lut_alnum_upper;
  if (b < 0) (b = -b, nlt = _plos_lut_alnum_lower);
  char *s = buf + buflen;
  *--s    = '\0';
  if (mpi_fhsb(_n, len) < 0) {
    *--s = '0';
    return s;
  }
  bool neg = _signed ? _n[len - 1] >> 63 : false;
  u64  n[len];
  if (neg)
    mpi_neg2(n, _n, len);
  else
    mpi_set(n, _n, len);
  u64 _b[len];
  mpi_set0(_b, len);
  _b[0] = b;
  u64 d[len];
  while (mpi_fhsb(n, len) >= 0) {
    mpi_div(n, d, n, _b, len);
    *--s = nlt[*d];
  }
  if (neg) *--s = '-';
  return s;
}

#  endif

#endif
