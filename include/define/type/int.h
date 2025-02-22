// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#if __INT_WIDTH__ != 32
#  error "int 必须为 int32"
#endif

#if __FLT_RADIX__ != 2
#  error "浮点数必须是二进制的"
#endif

#if __FLT_MANT_DIG__ != 24
#  error "float 必须为 32 位浮点数"
#endif

#if __DBL_MANT_DIG__ != 53
#  error "double 必须为 64 位浮点数"
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 基本类型

typedef __INTPTR_TYPE__  intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;
typedef __INTPTR_TYPE__  ssize_t;
typedef __UINTPTR_TYPE__ size_t;
typedef __INTPTR_TYPE__  ptrdiff_t;
typedef size_t           usize;
typedef ssize_t          isize;

typedef signed char        schar; // 在大多数环境下 schar 就是 char
typedef unsigned char      uchar;
typedef unsigned short     ushort;
typedef unsigned int       uint;
typedef unsigned long      ulong;
typedef long long          llong;
typedef unsigned long long ullong;

#ifndef __cplusplus
typedef __UINT8_TYPE__ char8_t; // 我们认为 char8 就是 uint8
typedef __CHAR16_TYPE__ char16_t;
typedef __CHAR32_TYPE__ char32_t;
typedef __WCHAR_TYPE__ wchar_t;
#endif

typedef __INT8_TYPE__   int8_t;
typedef __UINT8_TYPE__  uint8_t;
typedef __INT16_TYPE__  int16_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __INT32_TYPE__  int32_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT64_TYPE__  int64_t;
typedef __UINT64_TYPE__ uint64_t;
typedef float           float32_t;
typedef double          float64_t;
#if defined(__x86_64__)
typedef __int128          int128_t;
typedef unsigned __int128 uint128_t;
#  if !NO_EXTFLOAT
typedef _Float16   float16_t; // 命名最不统一的一集
typedef __float128 float128_t;
#  endif
#endif

typedef __INTMAX_TYPE__  intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 自定义别名

typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t   i16;
typedef uint16_t  u16;
typedef int32_t   i32;
typedef uint32_t  u32;
typedef int64_t   i64;
typedef uint64_t  u64;
typedef float32_t f32;
typedef float64_t f64;
#if defined(__x86_64__)
typedef int128_t  i128;
typedef uint128_t u128;
#  if !NO_EXTFLOAT
typedef float16_t  f16;
typedef float128_t f128;
#  endif
#endif

typedef intmax_t  imax_t;
typedef uintmax_t umax_t;
typedef intmax_t  imax;
typedef uintmax_t umax;
