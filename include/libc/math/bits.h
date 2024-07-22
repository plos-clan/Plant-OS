#pragma once
#include <define.h>
#include <type.h>

// --------------------------------------------------
//; clz

#ifndef __cplusplus
#  if __has(clz)
#    if defined(__GNUC__) && !defined(__clang__)
#      define clz(x)                                                                               \
        _Generic((x),                                                                              \
            unsigned char: __builtin_clz((uint)(x)) - 24,                                          \
            unsigned short: __builtin_clz((uint)(x)) - 16,                                         \
            unsigned int: __builtin_clz(x),                                                        \
            unsigned long: __builtin_clzl(x),                                                      \
            unsigned long long: __builtin_clzll(x))
#    else
#      define clz(x)                                                                               \
        _Generic((x),                                                                              \
            unsigned char: __builtin_clzs((ushort)(x)) - 8,                                        \
            unsigned short: __builtin_clzs(x),                                                     \
            unsigned int: __builtin_clz(x),                                                        \
            unsigned long: __builtin_clzl(x),                                                      \
            unsigned long long: __builtin_clzll(x))
#    endif
#  else
#    define __(TYPE, NAME)                                                                         \
      static int _##NAME(TYPE x) {                                                                 \
        int  count = 0;                                                                            \
        TYPE mask  = (TYPE)1 << (sizeof(TYPE) - 1);                                                \
        for (; mask && (x & mask) == 0; count++, mask = mask >> 1) {}                              \
        return count;                                                                              \
      }
__(ushort, clzs)
__(uint, clz)
__(ulong, clzl)
__(ullong, clzll)
#    undef __
#    define clz(x)                                                                                 \
      _Generic((x),                                                                                \
          unsigned char: _clzs((ushort)(x)) - 8,                                                   \
          unsigned short: _clzs(x),                                                                \
          unsigned int: _clz(x),                                                                   \
          unsigned long: _clzl(x),                                                                 \
          unsigned long long: _clzll(x))
#  endif
#endif

// --------------------------------------------------
//; 位逆序

#ifndef __cplusplus
finline u8 bit_reverse_8(u8 x) {
  x = ((x & 0x55) << 1) | ((x >> 1) & 0x55);
  x = ((x & 0x33) << 2) | ((x >> 2) & 0x33);
  x = ((x & 0x0f) << 4) | ((x >> 4) & 0x0f);
  return x;
}
finline u16 bit_reverse_16(u16 x) {
  x = ((x & 0x5555) << 1) | ((x >> 1) & 0x5555);
  x = ((x & 0x3333) << 2) | ((x >> 2) & 0x3333);
  x = ((x & 0x0f0f) << 4) | ((x >> 4) & 0x0f0f);
  x = ((x & 0x00ff) << 8) | ((x >> 8) & 0x00ff);
  return x;
}
finline u32 bit_reverse_32(u32 x) {
  x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
  x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
  x = ((x & 0x0f0f0f0f) << 4) | ((x >> 4) & 0x0f0f0f0f);
  x = ((x & 0x00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff);
  x = (x << 16) | (x >> 16);
  return x;
}
finline u64 bit_reverse_64(u64 x) {
  x = ((x & 0x5555555555555555) << 1) | ((x >> 1) & 0x5555555555555555);
  x = ((x & 0x3333333333333333) << 2) | ((x >> 2) & 0x3333333333333333);
  x = ((x & 0x0f0f0f0f0f0f0f0f) << 4) | ((x >> 4) & 0x0f0f0f0f0f0f0f0f);
  x = ((x & 0x00ff00ff00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff00ff00ff);
  x = ((x & 0x0000ffff0000ffff) << 16) | ((x >> 16) & 0x0000ffff0000ffff);
  x = (x << 32) | (x >> 32);
  return x;
}
#  define bit_reverse(x)                                                                           \
    _Generic((x),                                                                                  \
        u8: bit_reverse_8(x),                                                                      \
        u16: bit_reverse_16(x),                                                                    \
        u32: bit_reverse_32(x),                                                                    \
        u64: bit_reverse_64(x))
#endif

// --------------------------------------------------
//; 字节逆序

#ifndef __cplusplus
finline u8 byteswap8(u8 x) {
  return x;
}
finline u16 byteswap16(u16 x) {
  x = ((x & 0x00ff) << 8) | ((x >> 8) & 0x00ff);
  return x;
}
finline u32 byteswap32(u32 x) {
  x = ((x & 0x00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff);
  x = (x << 16) | (x >> 16);
  return x;
}
finline u64 byteswap64(u64 x) {
  x = ((x & 0x00ff00ff00ff00ff) << 8) | ((x >> 8) & 0x00ff00ff00ff00ff);
  x = ((x & 0x0000ffff0000ffff) << 16) | ((x >> 16) & 0x0000ffff0000ffff);
  x = (x << 32) | (x >> 32);
  return x;
}
#  define byteswap(x)                                                                              \
    _Generic((x), u8: byteswap8(x), u16: byteswap16(x), u32: byteswap32(x), u64: byteswap64(x))
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define little_endian(x) (x)
#    define big_endian(x)    (byteswap(x))
#  else
#    define little_endian(x) (byteswap(x))
#    define big_endian(x)    (x)
#  endif
#endif
