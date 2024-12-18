// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "int.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 编译器内置向量类型

#define __(type, len) typedef type vec##len##type __attribute__((vector_size(sizeof(type) * len)))

#define __8(type)                                                                                  \
  __(type, 2);                                                                                     \
  __(type, 4);                                                                                     \
  __(type, 8);
#define __16(type)                                                                                 \
  __8(type);                                                                                       \
  __(type, 16);
#define __32(type)                                                                                 \
  __16(type);                                                                                      \
  __(type, 32);
#define __64(type)                                                                                 \
  __32(type);                                                                                      \
  __(type, 64);
#define __128(type)                                                                                \
  __64(type);                                                                                      \
  __(type, 128);

__128(int8_t);
__128(uint8_t);
__64(int16_t);
__64(uint16_t);
__32(int32_t);
__32(uint32_t);
__16(int64_t);
__16(uint64_t);
__32(float32_t);
__16(float64_t);
#if defined(__x86_64__)
__8(int128_t);
__8(uint128_t);
#  if !NO_EXTFLOAT
__64(float16_t);
__8(float128_t);
#  endif
#endif

__32(float);
__16(double);

#undef __16
#undef __32
#undef __64
#undef __128

#undef __

//~ 自定义

#define __(type, len) typedef type v##len##type __attribute__((vector_size(sizeof(type) * len)))

#define __8(type)                                                                                  \
  __(type, 2);                                                                                     \
  __(type, 4);                                                                                     \
  __(type, 8);
#define __16(type)                                                                                 \
  __8(type);                                                                                       \
  __(type, 16);
#define __32(type)                                                                                 \
  __16(type);                                                                                      \
  __(type, 32);
#define __64(type)                                                                                 \
  __32(type);                                                                                      \
  __(type, 64);
#define __128(type)                                                                                \
  __64(type);                                                                                      \
  __(type, 128);

__128(i8);
__128(u8);
__64(i16);
__64(u16);
__32(i32);
__32(u32);
__16(i64);
__16(u64);
__32(f32);
__16(f64);
#if defined(__x86_64__)
__8(i128);
__8(u128);
#  if !NO_EXTFLOAT
__64(f16);
__8(f128);
#  endif
#endif

#undef __16
#undef __32
#undef __64
#undef __128

#undef __
