// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "int.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 复数

typedef _Complex float  cfloat;
typedef _Complex double cdouble;
typedef _Complex double complex;

typedef _Complex __INT8_TYPE__   cint8_t;
typedef _Complex __UINT8_TYPE__  cuint8_t;
typedef _Complex __INT16_TYPE__  cint16_t;
typedef _Complex __UINT16_TYPE__ cuint16_t;
typedef _Complex __INT32_TYPE__  cint32_t;
typedef _Complex __UINT32_TYPE__ cuint32_t;
typedef _Complex __INT64_TYPE__  cint64_t;
typedef _Complex __UINT64_TYPE__ cuint64_t;
typedef _Complex float           cfloat32_t;
typedef _Complex double          cfloat64_t;
#if !NO_EXTFLOAT
#  if defined(__x86_64__) && __GCC__ > 12
typedef _Complex _Float16 cfloat16_t;
#    ifdef __clang__
typedef _Complex __float128 cfloat128_t;
#    endif
#  endif
#endif

typedef cint8_t    ci8;
typedef cuint8_t   cu8;
typedef cint16_t   ci16;
typedef cuint16_t  cu16;
typedef cint32_t   ci32;
typedef cuint32_t  cu32;
typedef cint64_t   ci64;
typedef cuint64_t  cu64;
typedef cfloat32_t cf32;
typedef cfloat64_t cf64;
#if !NO_EXTFLOAT
#  if defined(__x86_64__) && __GCC__ > 12
typedef cfloat16_t cf16;
#    ifdef __clang__
typedef cfloat128_t cf128;
#    endif
#  endif
#endif
