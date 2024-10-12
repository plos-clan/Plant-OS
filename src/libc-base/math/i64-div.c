// This code is released under the MIT License

#include <libc-base.h>

// 在 32 位下支持 64 位整数除法
#if NO_STD && defined __i686__

dlexport void __udivmoddi4(u64 a, u64 b, u64 *_c, u64 *_d) {
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

dlexport void __divmoddi4(i64 a, i64 b, i64 *_c, i64 *_d) {
  bool c_neg = (a < 0) ^ (b < 0);
  bool d_neg = a < 0;
  if (a < 0) a = -a;
  if (b < 0) b = -b;
  u64 c, d;
  __udivmoddi4(a, b, &c, &d);
  if (_c) *_c = c_neg ? -c : c;
  if (_d) *_d = d_neg ? -d : d;
}

dlexport u64 __udivdi3(u64 a, u64 b) {
  u64 c;
  __udivmoddi4(a, b, &c, null);
  return c;
}

dlexport u64 __umoddi3(u64 a, u64 b) {
  u64 d;
  __udivmoddi4(a, b, null, &d);
  return d;
}

dlexport i64 __divdi3(i64 a, i64 b) {
  i64 c;
  __divmoddi4(a, b, &c, null);
  return c;
}

dlexport i64 __moddi3(i64 a, i64 b) {
  i64 d;
  __divmoddi4(a, b, null, &d);
  return d;
}

#endif
