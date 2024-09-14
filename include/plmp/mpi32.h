#include <libc-base.h>

#ifndef __x86_64__

ssize_t mpi_fhsb(u32 *a, size_t len);

void mpi_set0(u32 *a, size_t len) {
  memset(a, 0, len * 4);
}

// a = b
void mpi_set(u32 *a, u32 *b, size_t len) {
  memcpy(a, b, len * 4);
}

// a += b
bool mpi_add2(u32 *a, u32 *b, size_t len) {
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_uadd_overflow(a[i], overflow, &a[i]);
    overflow |= __builtin_uadd_overflow(a[i], b[i], &a[i]);
  }
  return overflow;
}
// c = a + b
bool mpi_add3(u32 *c, u32 *a, u32 *b, size_t len) {
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_uadd_overflow(a[i], overflow, &c[i]);
    overflow |= __builtin_uadd_overflow(c[i], b[i], &c[i]);
  }
  return overflow;
}

// a -= b
bool mpi_sub2(u32 *a, u32 *b, size_t len) {
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_usub_overflow(a[i], overflow, &a[i]);
    overflow |= __builtin_usub_overflow(a[i], b[i], &a[i]);
  }
  return overflow;
}
// c = a - b
bool mpi_sub3(u32 *c, u32 *a, u32 *b, size_t len) {
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_usub_overflow(a[i], overflow, &c[i]);
    overflow |= __builtin_usub_overflow(c[i], b[i], &c[i]);
  }
  return overflow;
}

// a *= b
bool mpi_mul2(u32 *a, u32 *b, size_t len) {
  bool overflow = false;
  //   for (size_t i = 0; i < len; i++) {
  //     overflow = __builtin_add_overflow(a[i], overflow, &a[i]);
  //     overflow |= __builtin_add_overflow(a[i], b[i], &a[i]);
  //   }
  return overflow;
}

// a <<= b
void mpi_lshift2(u32 *a, size_t b, size_t len) {
  size_t _b  = b / 32;
  b         &= 31;
  for (size_t i = len - 1; i > _b; i--) {
    a[i] = (a[i - _b] << b) | (a[i - _b - 1] >> (32 - b));
  }
  a[_b] = a[0] << b;
  for (ssize_t i = _b - 1; i >= 0; i--)
    a[i] = 0;
}
// c = a << b
void mpi_lshift3(u32 *c, u32 *a, size_t b, size_t len) {
  size_t _b  = b / 32;
  b         &= 31;
  for (size_t i = 0; i < _b; i++)
    c[i] = 0;
  c[_b] = a[0] << b;
  for (size_t i = _b + 1; i < len; i++) {
    c[i] = (a[i - _b] << b) | (a[i - _b - 1] >> (32 - b));
  }
}

// a >>= b
u32 mpi_rshift2(u32 *a, byte b, size_t len) {
  u32 overflow = 0;
  for (ssize_t i = len - 1; i >= 0; i--) {
    u32 o    = a[i] << (32 - b);
    a[i]     = (a[i] >> b) | overflow;
    overflow = o;
  }
  return overflow;
}
// c = a >> b
u32 mpi_rshift3(u32 *c, u32 *a, byte b, size_t len) {
  u32 overflow = 0;
  for (ssize_t i = len - 1; i >= 0; i--) {
    c[i]     = (a[i] >> b) | overflow;
    overflow = a[i] << (32 - b);
  }
  return overflow;
}

// c = a / b  d = a % b
void mpi_div(u32 *_rest c, u32 *_rest d, u32 *_rest a, u32 *_rest b, size_t len) {
  memcpy(d, a, len * 4);
  memset(c, 0, len * 4);
  ssize_t hba = mpi_fhsb(a, len);
  ssize_t hbb = mpi_fhsb(b, len);
  if (hba < hbb) return;
  int h = hba - hbb;
  if (h) mpi_lshift2(b, h, len);
  for (;; h--) {
    if (mpi_sub2(d, b, len))
      mpi_add2(d, b, len);
    else {
      printf("h: %d\n", h);
      c[h / 32] |= (u32)1 << (h & 31);
    }
    if (h == 0) break;
    mpi_rshift2(b, 1, len);
  }
}

// 比较数字
int mpi_cmp(u32 *a, u32 *b, size_t len) {
  for (ssize_t i = len - 1; i >= 0; i--) {
    if (a[i] < b[i]) return -1;
    if (a[i] > b[i]) return 1;
  }
  return 0;
}

// a = -a
void mpi_neg(u32 *a, size_t len) {
  for (size_t i = 0; i < len; i++)
    a[i] = ~a[i];
  bool overflow = true;
  for (size_t i = 0; i < len && overflow; i++)
    overflow = __builtin_uadd_overflow(a[i], 1, &a[i]);
}
// b = -a
void mpi_neg2(u32 *b, u32 *a, size_t len) {
  for (size_t i = 0; i < len; i++)
    b[i] = ~a[i];
  bool overflow = true;
  for (size_t i = 0; i < len && overflow; i++)
    overflow = __builtin_uadd_overflow(b[i], 1, &b[i]);
}

// fhsb(a)
ssize_t mpi_fhsb(u32 *a, size_t len) {
  ssize_t n;
  for (ssize_t i = len - 1; i; i--)
    if ((n = fhsb32(a[i])) >= 0) return n + i * 32;
  return fhsb32(a[0]);
}

extern const char _plos_lut_alnum_lower[62];
extern const char _plos_lut_alnum_upper[62];

char *mpi_tostr(char *buf, size_t buflen, u32 *_n, size_t len, int b, bool _signed) {
  cstr nlt = _plos_lut_alnum_upper;
  if (b < 0) (b = -b, nlt = _plos_lut_alnum_lower);
  char *s = buf + buflen;
  *--s    = '\0';
  if (mpi_fhsb(_n, len) < 0) {
    *--s = '0';
    return s;
  }
  bool neg = _signed ? _n[len - 1] >> 31 : false;
  u32  n1[len];
  u32  n2[len];
  u32 *n = n1;
  if (neg)
    mpi_neg2(n, _n, len);
  else
    mpi_set(n, _n, len);
  u32 _b[len];
  mpi_set0(_b, len);
  _b[0] = b;
  u32 d[len];
  while (mpi_fhsb(n, len) >= 0) {
    mpi_div(n == n1 ? n2 : n1, d, n, _b, len);
    printf("%d\n", *d);
    *--s = nlt[*d];
    n    = n == n1 ? n2 : n1;
  }
  if (neg) *--s = '-';
  return s;
}

#endif
