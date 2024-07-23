#pragma once
#include "base.hpp"
#include <define.h>
#include <type.hpp>

namespace cpp {

// --------------------------------------------------
//; clz

#if __has(clz)
#  if defined(__GNUC__) && !defined(__clang__)
finline auto clz(u8 x) -> int {
  return __builtin_clz((u32)x) - 24;
}
finline auto clz(u16 x) -> int {
  return __builtin_clz((u32)x) - 16;
}
#  else
finline auto clz(u8 x) -> int {
  return __builtin_clzs((u16)(x)) - 8;
}
finline auto clz(u16 x) -> int {
  return __builtin_clzs(x);
}
#  endif
finline auto clz(u32 x) -> int {
  return __builtin_clz(x);
}
#  if defined(__LP64__)
finline auto clz(u64 x) -> int {
  return __builtin_clzl(x);
}
#  else
finline auto clz(u64 x) -> int {
  return __builtin_clzll(x);
}
#  endif
#else
#  define __(TYPE, NAME)                                                                           \
    static auto clz(TYPE x)->int {                                                                 \
      int  count = 0;                                                                              \
      TYPE mask  = (TYPE)1 << (sizeof(TYPE) - 1);                                                  \
      for (; mask && (x & mask) == 0; count++, mask = mask >> 1) {}                                \
      return count;                                                                                \
    }
__(u8, clz)
__(u16, clz)
__(u32, clzl)
__(u64, clzll)
#  undef __
#endif

// --------------------------------------------------
//; 位逆序

finline auto bit_reverse(u8 x) {
  x = ((x & 0x55) << 1) | ((x >> 1) & 0x55);
  x = ((x & 0x33) << 2) | ((x >> 2) & 0x33);
  x = ((x & 0x0f) << 4) | ((x >> 4) & 0x0f);
  return x;
}
finline auto bit_reverse(u16 x) {
  x = ((x & 0x5555) << 1) | ((x >> 1) & 0x5555);
  x = ((x & 0x3333) << 2) | ((x >> 2) & 0x3333);
  x = ((x & 0x0f0f) << 4) | ((x >> 4) & 0x0f0f);
  x = ((x & 0x00ff) << 8) | ((x >> 8) & 0x00ff);
  return x;
}
finline auto bit_reverse(u32 x) {
  x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
  x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
  x = ((x & 0x0f0f0f0f) << 4) | ((x >> 4) & 0x0f0f0f0f);
  x = ((x & 0x00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff);
  x = (x << 16) | (x >> 16);
  return x;
}
finline auto bit_reverse(u64 x) {
  x = ((x & 0x5555555555555555) << 1) | ((x >> 1) & 0x5555555555555555);
  x = ((x & 0x3333333333333333) << 2) | ((x >> 2) & 0x3333333333333333);
  x = ((x & 0x0f0f0f0f0f0f0f0f) << 4) | ((x >> 4) & 0x0f0f0f0f0f0f0f0f);
  x = ((x & 0x00ff00ff00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff00ff00ff);
  x = ((x & 0x0000ffff0000ffff) << 16) | ((x >> 16) & 0x0000ffff0000ffff);
  x = (x << 32) | (x >> 32);
  return x;
}

// --------------------------------------------------
//; 字节逆序

finline auto byteswap(u8 x) {
  return x;
}
finline auto byteswap(u16 x) {
  x = ((x & 0x00ff) << 8) | ((x >> 8) & 0x00ff);
  return x;
}
finline auto byteswap(u32 x) {
  x = ((x & 0x00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff);
  x = (x << 16) | (x >> 16);
  return x;
}
finline auto byteswap(u64 x) {
  x = ((x & 0x00ff00ff00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff00ff00ff);
  x = ((x & 0x0000ffff0000ffff) << 16) | ((x >> 16) & 0x0000ffff0000ffff);
  x = (x << 32) | (x >> 32);
  return x;
}
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define little_endian(x) (x)
#  define big_endian(x)    (byteswap(x))
#else
#  define little_endian(x) (byteswap(x))
#  define big_endian(x)    (x)
#endif

} // namespace cpp
