#include <libc.h>

dlexport void u64_div(u64 a, u64 b, u64 *_c, u64 *_d) {
  u64 c    = 0;
  u64 mask = 1;
  while (a > b && (b & (U64_MAX + 1)) == 0) {
    b    = b << 1;
    mask = mask << 1;
  }
  while (mask > 0) {
    if (a > b) {
      a -= b;
      c |= mask;
    }
    b    = b >> 1;
    mask = mask >> 1;
  }
  if (_c) *_c = c;
  if (_d) *_d = a;
}

dlexport void i64_div(i64 a, i64 b, i64 *_c, i64 *_d) {
  bool c_neg = (a < 0) ^ (b < 0);
  bool d_neg = a < 0;
  if (a < 0) a = -a;
  if (b < 0) b = -b;
  u64 c, d;
  u64_div(a, b, &c, &d);
  if (_c) *_c = c_neg ? -c : c;
  if (_d) *_d = d_neg ? -d : d;
}

dlexport u64 __udivdi3(u64 a, u64 b) {
  u64 c;
  u64_div(a, b, &c, null);
  return c;
}

dlexport u64 __umoddi3(u64 a, u64 b) {
  u64 d;
  u64_div(a, b, null, &d);
  return d;
}

dlexport i64 __divdi3(i64 a, i64 b) {
  i64 c;
  i64_div(a, b, &c, null);
  return c;
}

dlexport i64 __moddi3(i64 a, i64 b) {
  i64 d;
  i64_div(a, b, null, &d);
  return d;
}
