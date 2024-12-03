#include <libc-base.h>

#define LEN_MPI(_nbits_)     (((_nbits_) + 31) / 32)
#define MPI(_nbits_, _name_) u32 _name_[LEN_MPI(_nbits_)]

//* ----------------------------------------------------------------------------------------------------
//; 声明
//* ----------------------------------------------------------------------------------------------------

#define API finline __THROW

#define RD(...) __attr_readonly(__VA_ARGS__)
#define WR(...) __attr_writeonly(__VA_ARGS__)

#define ATTR_1_M    __nnull(1) WR(1, 2)
#define ATTR_1_C    __nnull(1) RD(1, 2)
#define ATTR_2_MC   __nnull(1, 2) WR(1, 3) RD(2, 3)
#define ATTR_2_MU   __nnull(1) WR(1, 3)
#define ATTR_2_MI   __nnull(1) WR(1, 3)
#define ATTR_2_CC   __nnull(1, 2) RD(1, 3) RD(2, 3)
#define ATTR_3_MCC  __nnull(1, 2, 3) WR(1, 4) RD(2, 4) RD(3, 4)
#define ATTR_3_MCU  __nnull(1, 2) WR(1, 4) RD(2, 4)
#define ATTR_3_MCI  __nnull(1, 2) WR(1, 4) RD(2, 4)
#define ATTR_4_MMCC __nnull(1, 2, 3, 4) WR(1, 5) WR(2, 5) RD(3, 5) RD(4, 5)

#define ATTR_1 ATTR_1_M
#define ATTR_2 ATTR_2_MC
#define ATTR_3 ATTR_3_MCC
#define ATTR_4 ATTR_4_MMCC

#define ARGS_1_M    u32 *_rest a, size_t len                     // a = [constant] or a = [op] a
#define ARGS_1_C    const u32 *_rest a, size_t len               // [op] a
#define ARGS_2_MC   u32 *_rest a, const u32 *_rest b, size_t len // a [op]= b
#define ARGS_2_MU   u32 *_rest a, u32 b, size_t len              // a [op]= b
#define ARGS_2_MI   u32 *_rest a, i32 b, size_t len              // a [op]= b
#define ARGS_2_CC   const u32 *_rest a, const u32 *_rest b, size_t len               // a [op] b
#define ARGS_3_MCC  u32 *_rest c, const u32 *_rest a, const u32 *_rest b, size_t len // c = a [op] b
#define ARGS_3_MCU  u32 *_rest c, const u32 *_rest a, u32 b, size_t len              // c = a [op] b
#define ARGS_3_MCI  u32 *_rest c, const u32 *_rest a, i32 b, size_t len              // c = a [op] b
#define ARGS_4_MMCC u32 *_rest c, u32 *_rest d, const u32 *_rest a, const u32 *_rest b, size_t len

#define ARGS_1 ARGS_1_M
#define ARGS_2 ARGS_2_MC
#define ARGS_3 ARGS_3_MCC
#define ARGS_4 ARGS_4_MMCC

#define ATTR_SETS __nnull(1, 3) WR(1, 2)
#define ARGS_SETS u32 *a, size_t len, cstr b, int base

// --------------------------------------------------
//; 基本函数

API void mpi_set0(ARGS_1) ATTR_1;       // a = 0
API void mpi_set1(ARGS_1) ATTR_1;       // a = 1
API void mpi_set(ARGS_2) ATTR_2;        // a = b
API void mpi_setu(ARGS_2_MU) ATTR_2_MU; // a = b
API void mpi_seti(ARGS_2_MI) ATTR_2_MI; // a = b
API int  mpi_sets(ARGS_SETS) ATTR_SETS; // a = mpi(b)

API void mpi_neg(ARGS_1) ATTR_1;  // a = -a
API void mpi_neg2(ARGS_2) ATTR_2; // a = -b

API isize mpi_fhsb(ARGS_1_C) __attr(pure) ATTR_1_C; // 最高有效位的索引

API int mpi_cmp(ARGS_2_CC) __attr(pure) ATTR_2_CC; // 比较数字

// --------------------------------------------------
//; 加减乘除

API bool mpi_add2(ARGS_2) ATTR_2;          // a += b
API bool mpi_add2u(ARGS_2_MU) ATTR_2_MU;   // a += b
API bool mpi_add2i(ARGS_2_MI) ATTR_2_MI;   // a += b
API bool mpi_add3(ARGS_3) ATTR_3;          // c = a + b
API bool mpi_add3u(ARGS_3_MCU) ATTR_3_MCU; // c = a + b
API bool mpi_add3i(ARGS_3_MCI) ATTR_3_MCI; // c = a + b

API bool mpi_sub2(ARGS_2) ATTR_2;          // a -= b
API bool mpi_sub2u(ARGS_2_MU) ATTR_2_MU;   // a -= b
API bool mpi_sub2i(ARGS_2_MI) ATTR_2_MI;   // a -= b
API bool mpi_sub3(ARGS_3) ATTR_3;          // c = a - b
API bool mpi_sub3u(ARGS_3_MCU) ATTR_3_MCU; // c = a - b
API bool mpi_sub3i(ARGS_3_MCI) ATTR_3_MCI; // c = a - b

API bool mpi_mul2(ARGS_2) ATTR_2;          // a *= b
API bool mpi_mul2u(ARGS_2_MU) ATTR_2_MU;   // a *= b
API bool mpi_mul2i(ARGS_2_MI) ATTR_2_MI;   // a *= b
API bool mpi_mul3(ARGS_3) ATTR_3;          // c = a * b
API bool mpi_mul3u(ARGS_3_MCU) ATTR_3_MCU; // c = a * b
API bool mpi_mul3i(ARGS_3_MCI) ATTR_3_MCI; // c = a * b

API void mpi_div(ARGS_4) ATTR_4;

// --------------------------------------------------
//; 位运算

API void mpi_lshift2(ARGS_2_MU) ATTR_2_MU;   // a <<= b
API void mpi_lshift3(ARGS_3_MCU) ATTR_3_MCU; // c = a << b
API void mpi_rshift2(ARGS_2_MU) ATTR_2_MU;   // a >>= b
API void mpi_rshift3(ARGS_3_MCU) ATTR_3_MCU; // c = a >> b

#undef WR
#undef RD

#undef API

//* ----------------------------------------------------------------------------------------------------
//; 定义
//* ----------------------------------------------------------------------------------------------------

#define API finline

// --------------------------------------------------
//; 基本函数

API void mpi_set0(ARGS_1) noexcept { // a = 0
  memset(a, 0, len * 4);
}
API void mpi_set1(ARGS_1) noexcept { // a = 1
  memset(a, 0, len * 4);
  a[0] = 1;
}
API void mpi_set(ARGS_2) noexcept { // a = b
  memcpy(a, b, len * 4);
}
API void mpi_setu(ARGS_2_MU) noexcept { // a = b
  memset(a, 0, len * 4);
  a[0] = b;
}
API void mpi_seti(ARGS_2_MI) noexcept { // a = b
  memset(a, b < 0 ? 0xff : 0, len * 4);
  a[0] = b;
}
API int mpi_sets(ARGS_SETS) noexcept { // a = mpi(b)
  assert(2 <= base && base <= 36);
  memset(a, 0, len * 4);
  for (cstr p = b; *p; p++) {
    u32 c = (u32)*p;
    if ('0' <= c && c <= '9') {
      c -= '0';
    } else if ('a' <= c && c <= 'z') {
      c -= 'a' - 10;
    } else if ('A' <= c && c <= 'Z') {
      c -= 'A' - 10;
    } else {
      return -1;
    }
    mpi_mul2u(a, base, len);
    mpi_add2u(a, c, len);
  }
  return 0;
}

API void mpi_neg(ARGS_1) noexcept { // a = -a
  for (size_t i = 0; i < len; i++) {
    a[i] = ~a[i];
  }
  bool overflow = true;
  for (size_t i = 0; overflow && i < len; i++) {
    overflow = __builtin_uadd_overflow(a[i], 1, &a[i]);
  }
}
API void mpi_neg2(ARGS_2) noexcept { // a = -b
  for (size_t i = 0; i < len; i++) {
    a[i] = ~b[i];
  }
  bool overflow = true;
  for (size_t i = 0; overflow && i < len; i++) {
    overflow = __builtin_uadd_overflow(b[i], 1, &b[i]);
  }
}

API isize mpi_fhsb(ARGS_1_C) noexcept { // 最高有效位的索引
  for (isize i = len - 1; i > 0; i--) {
    isize n = fhsb32(a[i]);
    if (n >= 0) return n + i * 32;
  }
  return fhsb32(a[0]);
}

API int mpi_cmp(ARGS_2_CC) noexcept { // 比较数字
  for (ssize_t i = len - 1; i >= 0; i--) {
    if (a[i] < b[i]) return -1;
    if (a[i] > b[i]) return 1;
  }
  return 0;
}

// --------------------------------------------------
//; 加减

API bool mpi_add2(ARGS_2) noexcept { // a += b
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_uadd_overflow(a[i], overflow, &a[i]);
    overflow |= __builtin_uadd_overflow(a[i], b[i], &a[i]);
  }
  return overflow;
}
API bool mpi_add2u(ARGS_2_MU) noexcept { // a += b
  if (len == 0) return b != 0;
  bool overflow = __builtin_uadd_overflow(a[0], b, &a[0]);
  for (size_t i = 1; overflow && i < len; i++) {
    overflow = __builtin_uadd_overflow(a[i], overflow, &a[i]);
  }
  return overflow;
}
API bool mpi_add2i(ARGS_2_MI) noexcept { // a += b
  return b < 0 ? mpi_sub2u(a, -b, len) : mpi_add2u(a, b, len);
}
API bool mpi_add3(ARGS_3) noexcept { // c = a + b
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_uadd_overflow(a[i], overflow, &c[i]);
    overflow |= __builtin_uadd_overflow(c[i], b[i], &c[i]);
  }
  return overflow;
}
API bool mpi_add3u(ARGS_3_MCU) noexcept { // c = a + b
  if (len == 0) return b != 0;
  bool overflow = __builtin_uadd_overflow(a[0], b, &c[0]);
  for (size_t i = 1; i < len; i++) {
    if (overflow) {
      overflow = __builtin_uadd_overflow(a[i], overflow, &c[i]);
    } else {
      c[i] = a[i];
    }
  }
  return overflow;
}
API bool mpi_add3i(ARGS_3_MCI) noexcept { // c = a + b
  return b < 0 ? mpi_sub3u(c, a, -b, len) : mpi_add3u(c, a, b, len);
}

API bool mpi_sub2(ARGS_2) noexcept { // a -= b
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_usub_overflow(a[i], overflow, &a[i]);
    overflow |= __builtin_usub_overflow(a[i], b[i], &a[i]);
  }
  return overflow;
}
API bool mpi_sub2u(ARGS_2_MU) noexcept { // a -= b
  if (len == 0) return b != 0;
  bool overflow = __builtin_usub_overflow(a[0], b, &a[0]);
  for (size_t i = 1; overflow && i < len; i++) {
    overflow = __builtin_usub_overflow(a[i], overflow, &a[i]);
  }
  return overflow;
}
API bool mpi_sub2i(ARGS_2_MI) noexcept { // a -= b
  return b < 0 ? mpi_add2u(a, -b, len) : mpi_sub2u(a, b, len);
}
API bool mpi_sub3(ARGS_3) noexcept { // c = a - b
  bool overflow = false;
  for (size_t i = 0; i < len; i++) {
    overflow  = __builtin_usub_overflow(a[i], overflow, &c[i]);
    overflow |= __builtin_usub_overflow(c[i], b[i], &c[i]);
  }
  return overflow;
}
API bool mpi_sub3u(ARGS_3_MCU) noexcept { // c = a - b
  if (len == 0) return b != 0;
  bool overflow = __builtin_usub_overflow(a[0], b, &c[0]);
  for (size_t i = 1; i < len; i++) {
    if (overflow) {
      overflow = __builtin_usub_overflow(a[i], overflow, &c[i]);
    } else {
      c[i] = a[i];
    }
  }
  return overflow;
}
API bool mpi_sub3i(ARGS_3_MCI) noexcept { // c = a - b
  return b < 0 ? mpi_add3u(c, a, -b, len) : mpi_sub3u(c, a, b, len);
}

API bool mpi_mul2(ARGS_2) noexcept { // a *= b
  u32 c[len + 1] = {};
  for (size_t i = 0; i < len; i++) {
    for (size_t j = 0; j < len - i; j++) {
      u64 t         = (u64)a[i] * b[j] + c[i + j];
      c[i + j]      = (u32)t;
      c[i + j + 1] += t >> 32;
    }
  }
  memcpy(a, c, len * 4);
  return c[len] != 0;
}
API bool mpi_mul2u(ARGS_2_MU) noexcept { // a *= b
  if (b == 0) {
    memset(a, 0, len * 4);
    return false;
  }
  u32 overflow = 0;
  for (size_t i = 0; i < len; i++) {
    u64 t    = (u64)a[i] * b + overflow;
    a[i]     = (u32)t;
    overflow = t >> 32;
  }
  return overflow != 0;
}
API bool mpi_mul2i(ARGS_2_MI) noexcept { // a *= b
  if (b < 0) {
    bool overflow = mpi_mul2u(a, -b, len);
    mpi_neg(a, len);
    return overflow;
  } else {
    return mpi_mul2u(a, b, len);
  }
}
API bool mpi_mul3(ARGS_3) noexcept { // c = a * b
  u32 overflow = 0;
  for (size_t i = 0; i < len; i++) {
    for (size_t j = 0; j < len - i; j++) {
      u64 t    = (u64)a[i] * b[j] + c[i + j];
      c[i + j] = (u32)t;
      if (i + j + 1 < len) c[i + j + 1] += t >> 32;
      if (i + j + 1 == len) overflow = t >> 32;
    }
  }
  return overflow != 0;
}
API bool mpi_mul3u(ARGS_3_MCU) noexcept { // c = a * b
  if (b == 0) {
    memset(c, 0, len * 4);
    return false;
  }
  u32 overflow = 0;
  for (size_t i = 0; i < len; i++) {
    u64 t    = (u64)a[i] * b + overflow;
    c[i]     = (u32)t;
    overflow = t >> 32;
  }
  return overflow != 0;
}
API bool mpi_mul3i(ARGS_3_MCI) noexcept { // c = a * b
  if (b < 0) {
    bool overflow = mpi_mul3u(c, a, -b, len);
    mpi_neg(c, len);
    return overflow;
  } else {
    return mpi_mul3u(c, a, b, len);
  }
}

API void mpi_lshift2(ARGS_2_MU) noexcept { // a <<= b
  u32 _b  = b / 32;
  b      &= 31;
  for (size_t i = len - 1; i > _b; i--) {
    a[i] = (a[i - _b] << b) | (a[i - _b - 1] >> (32 - b));
  }
  a[_b] = a[0] << b;
  for (size_t i = 0; i < _b; i++) {
    a[i] = 0;
  }
}
API void mpi_lshift3(ARGS_3_MCU) noexcept { // c = a << b
  u32 _b  = b / 32;
  b      &= 31;
  for (size_t i = 0; i < _b; i++) {
    c[i] = 0;
  }
  c[_b] = a[0] << b;
  for (size_t i = _b + 1; i < len; i++) {
    c[i] = (a[i - _b] << b) | (a[i - _b - 1] >> (32 - b));
  }
}
API void mpi_rshift2(ARGS_2_MU) noexcept { // a >>= b
  u32 _b  = b / 32;
  b      &= 31;
  for (size_t i = 0; i < len - _b - 1; i++) {
    a[i] = (a[i + _b] >> b) | (a[i + _b + 1] << (32 - b));
  }
  a[len - _b - 1] = a[len - 1] >> b;
  for (size_t i = len - _b; i < len; i++) {
    a[i] = 0;
  }
}
API void mpi_rshift3(ARGS_3_MCU) noexcept { // c = a >> b
  u32 _b  = b / 32;
  b      &= 31;
  for (size_t i = len - 1; i > _b; i--) {
    c[i] = (a[i - _b] >> b) | (a[i - _b - 1] << (32 - b));
  }
  c[_b] = a[len - 1] >> b;
  for (size_t i = 0; i < _b; i++) {
    c[i] = 0;
  }
}

// c = a / b  d = a % b
API void mpi_div(ARGS_4) noexcept {
  memcpy(d, a, len * 4);
  memset(c, 0, len * 4);
  ssize_t hba = mpi_fhsb(a, len);
  ssize_t hbb = mpi_fhsb(b, len);
  if (hba < hbb) return;
  int h = hba - hbb;
  if (h) mpi_lshift2(b, h, len);
  for (;; h--) {
    if (mpi_sub2(d, b, len)) {
      mpi_add2(d, b, len);
    } else {
      c[h / 32] |= (u32)1 << (h & 31);
    }
    if (h == 0) break;
    mpi_rshift2(b, 1, len);
  }
}

API char *mpi_tostr(char *buf, size_t buflen, const u32 *_n, size_t len, int b,
                    bool _signed) noexcept {
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
    *--s = nlt[*d];
    n    = n == n1 ? n2 : n1;
  }
  if (neg) *--s = '-';
  return s;
}

#undef API
