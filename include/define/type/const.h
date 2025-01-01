// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "base.h"
#include "complex.h"
#include "int.h"

#define _CONST_(_type_) typedef const _type_ $##_type_

#define $void const void

#define $auto val

_CONST_(bool);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 基本类型

_CONST_(intptr_t);
_CONST_(uintptr_t);
_CONST_(ssize_t);
_CONST_(size_t);
_CONST_(ptrdiff_t);
_CONST_(usize);
_CONST_(isize);

_CONST_(schar);
_CONST_(uchar);
_CONST_(ushort);
_CONST_(uint);
_CONST_(ulong);
_CONST_(llong);
_CONST_(ullong);

_CONST_(char8_t);
_CONST_(char16_t);
_CONST_(char32_t);
_CONST_(wchar_t);

_CONST_(int8_t);
_CONST_(uint8_t);
_CONST_(int16_t);
_CONST_(uint16_t);
_CONST_(int32_t);
_CONST_(uint32_t);
_CONST_(int64_t);
_CONST_(uint64_t);
_CONST_(float32_t);
_CONST_(float64_t);
#if defined(__x86_64__)
_CONST_(int128_t);
_CONST_(uint128_t);
#  if !NO_EXTFLOAT
_CONST_(float16_t);
_CONST_(float128_t);
#  endif
#endif

_CONST_(intmax_t);
_CONST_(uintmax_t);

_CONST_(i8);
_CONST_(u8);
_CONST_(i16);
_CONST_(u16);
_CONST_(i32);
_CONST_(u32);
_CONST_(i64);
_CONST_(u64);
_CONST_(f32);
_CONST_(f64);
#if defined(__x86_64__)
_CONST_(i128);
_CONST_(u128);
#  if !NO_EXTFLOAT
_CONST_(f16);
_CONST_(f128);
#  endif
#endif

_CONST_(imax_t);
_CONST_(umax_t);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 复数

_CONST_(cfloat);
_CONST_(cdouble);
_CONST_(complex);

_CONST_(cint8_t);
_CONST_(cuint8_t);
_CONST_(cint16_t);
_CONST_(cuint16_t);
_CONST_(cint32_t);
_CONST_(cuint32_t);
_CONST_(cint64_t);
_CONST_(cuint64_t);
_CONST_(cfloat32_t);
_CONST_(cfloat64_t);
#if !NO_EXTFLOAT
#  if defined(__x86_64__) && __GCC__ > 12
_CONST_(cfloat16_t);
#    ifdef __clang__
_CONST_(cfloat128_t);
#    endif
#  endif
#endif

_CONST_(ci8);
_CONST_(cu8);
_CONST_(ci16);
_CONST_(cu16);
_CONST_(ci32);
_CONST_(cu32);
_CONST_(ci64);
_CONST_(cu64);
_CONST_(cf32);
_CONST_(cf64);
#if !NO_EXTFLOAT
#  if defined(__x86_64__) && __GCC__ > 12
_CONST_(cf16);
#    ifdef __clang__
_CONST_(cf128);
#    endif
#  endif
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

_CONST_(sbyte);
_CONST_(byte);

_CONST_(cstr);

_CONST_(errno_t);

#undef _CONST_
