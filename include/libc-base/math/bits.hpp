#pragma once
#include "base.hpp"
#include <define.hpp>

namespace cpp {

// --------------------------------------------------
//; clz

#if __has(clz)
#  if defined(__GNUC__) && !defined(__clang__)
inline_const auto clz(u8 x) -> isize {
  return __builtin_clz((u32)x) - 24;
}
inline_const auto clz(u16 x) -> isize {
  return __builtin_clz((u32)x) - 16;
}
#  else
inline_const auto clz(u8 x) -> isize {
  return __builtin_clzs((u16)(x)) - 8;
}
inline_const auto clz(u16 x) -> isize {
  return __builtin_clzs(x);
}
#  endif
inline_const auto clz(u32 x) -> isize {
  return __builtin_clz(x);
}
#  if defined(__LP64__)
inline_const auto clz(u64 x) -> isize {
  return __builtin_clzl(x);
}
#  else
inline_const auto clz(u64 x) -> isize {
  return __builtin_clzll(x);
}
#  endif
#else
#  define __(TYPE)                                                                                 \
    inline_const auto clz(TYPE x) -> isize {                                                       \
      isize count = 0;                                                                             \
      TYPE  mask  = (TYPE)1 << (sizeof(TYPE) * 8 - 1);                                             \
      for (; mask && (x & mask) == 0; count++, mask = mask >> 1) {}                                \
      return count;                                                                                \
    }
__(u8)
__(u16)
__(u32)
__(u64)
#  undef __
#endif

// --------------------------------------------------
//; fhsb 返回 64 位无符号整数中最高有效位的索引，如果没有找到 1 位则返回 -1

inline_const auto fhsb(u8 x) -> isize {
  if (x == 0) return -1;
  return 7 - clz(x);
}
inline_const auto fhsb(u16 x) -> isize {
  if (x == 0) return -1;
  return 15 - clz(x);
}
inline_const auto fhsb(u32 x) -> isize {
  if (x == 0) return -1;
  return 31 - clz(x);
}
inline_const auto fhsb(u64 x) -> isize {
  if (x == 0) return -1;
  return 63 - clz(x);
}

// --------------------------------------------------
//; 位逆序

inline_const auto bit_reverse(u8 x) {
  x = ((x & 0x55) << 1) | ((x >> 1) & 0x55);
  x = ((x & 0x33) << 2) | ((x >> 2) & 0x33);
  x = ((x & 0x0f) << 4) | ((x >> 4) & 0x0f);
  return x;
}
inline_const auto bit_reverse(u16 x) {
  x = ((x & 0x5555) << 1) | ((x >> 1) & 0x5555);
  x = ((x & 0x3333) << 2) | ((x >> 2) & 0x3333);
  x = ((x & 0x0f0f) << 4) | ((x >> 4) & 0x0f0f);
  x = ((x & 0x00ff) << 8) | ((x >> 8) & 0x00ff);
  return x;
}
inline_const auto bit_reverse(u32 x) {
  x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
  x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
  x = ((x & 0x0f0f0f0f) << 4) | ((x >> 4) & 0x0f0f0f0f);
  x = ((x & 0x00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff);
  x = (x << 16) | (x >> 16);
  return x;
}
inline_const auto bit_reverse(u64 x) {
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

inline_const auto byteswap(u8 x) {
  return x;
}
inline_const auto byteswap(u16 x) {
  x = ((x & 0x00ff) << 8) | ((x >> 8) & 0x00ff);
  return x;
}
inline_const auto byteswap(u32 x) {
  x = ((x & 0x00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff);
  x = (x << 16) | (x >> 16);
  return x;
}
inline_const auto byteswap(u64 x) {
  x = ((x & 0x00ff00ff00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff00ff00ff);
  x = ((x & 0x0000ffff0000ffff) << 16) | ((x >> 16) & 0x0000ffff0000ffff);
  x = (x << 32) | (x >> 32);
  return x;
}
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define little_endian(x) (x)
#  define big_endian(x)    (::cpp::byteswap(x))
#else
#  define little_endian(x) (::cpp::byteswap(x))
#  define big_endian(x)    (x)
#endif

} // namespace cpp
