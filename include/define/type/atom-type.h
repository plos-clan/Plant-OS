// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "bool.h"
#include "int.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 标准库原子类型

//- 参考 gcc   /usr/lib/gcc/x86_64-linux-gnu/14/include/stdatomic.h
//- 参考 clang /usr/lib/llvm-19/lib/clang/19/include/stdatomic.h
//-   注意 clang 兼容 gcc 标准头文件，所以我们可以直接参照 gcc 的实现

#if NO_STD || !defined(__cplusplus)
#  define atomic _Atomic
#endif

#if defined(__cplusplus) && defined(__GNUC__) && !defined(__clang__)
//+ 使用 gcc 编译 C++ 代码

#  if 0

extern "C++" {
#    include <bits/atomic_base.h> // 这文件移植不过来 (gcc 寄!)
using std::__atomic_base;
}

typedef __atomic_base<bool>               atomic_bool;
typedef __atomic_base<char>               atomic_char;
typedef __atomic_base<signed char>        atomic_schar;
typedef __atomic_base<unsigned char>      atomic_uchar;
typedef __atomic_base<short>              atomic_short;
typedef __atomic_base<unsigned short>     atomic_ushort;
typedef __atomic_base<int>                atomic_int;
typedef __atomic_base<unsigned int>       atomic_uint;
typedef __atomic_base<long>               atomic_long;
typedef __atomic_base<unsigned long>      atomic_ulong;
typedef __atomic_base<long long>          atomic_llong;
typedef __atomic_base<unsigned long long> atomic_ullong;

typedef __atomic_base<int8_t>   atomic_int8_t;
typedef __atomic_base<uint8_t>  atomic_uint8_t;
typedef __atomic_base<int16_t>  atomic_int16_t;
typedef __atomic_base<uint16_t> atomic_uint16_t;
typedef __atomic_base<int32_t>  atomic_int32_t;
typedef __atomic_base<uint32_t> atomic_uint32_t;
typedef __atomic_base<int64_t>  atomic_int64_t;
typedef __atomic_base<uint64_t> atomic_uint64_t;
#    if defined(__x86_64__)
typedef __atomic_base<int128_t>  atomic_int128_t;
typedef __atomic_base<uint128_t> atomic_uint128_t;
#    endif

typedef __atomic_base<intmax_t>  atomic_intmax_t;
typedef __atomic_base<uintmax_t> atomic_uintmax_t;

typedef __atomic_base<char16_t> atomic_char16_t;
typedef __atomic_base<char32_t> atomic_char32_t;
typedef __atomic_base<wchar_t>  atomic_wchar_t;

typedef __atomic_base<intptr_t>  atomic_intptr_t;
typedef __atomic_base<uintptr_t> atomic_uintptr_t;
typedef __atomic_base<ssize_t>   atomic_ssize_t;
typedef __atomic_base<size_t>    atomic_size_t;
typedef __atomic_base<ptrdiff_t> atomic_ptrdiff_t;

typedef __atomic_base<bool> atomic_flag_t;

#  endif

#else
//+ 使用 gcc 编译 C 代码
//+ 使用 clang 编译

typedef _Atomic bool               atomic_bool;
typedef _Atomic char               atomic_char;
typedef _Atomic signed char        atomic_schar;
typedef _Atomic unsigned char      atomic_uchar;
typedef _Atomic short              atomic_short;
typedef _Atomic unsigned short     atomic_ushort;
typedef _Atomic int                atomic_int;
typedef _Atomic unsigned int       atomic_uint;
typedef _Atomic long               atomic_long;
typedef _Atomic unsigned long      atomic_ulong;
typedef _Atomic long long          atomic_llong;
typedef _Atomic unsigned long long atomic_ullong;

typedef _Atomic int8_t   atomic_int8_t;
typedef _Atomic uint8_t  atomic_uint8_t;
typedef _Atomic int16_t  atomic_int16_t;
typedef _Atomic uint16_t atomic_uint16_t;
typedef _Atomic int32_t  atomic_int32_t;
typedef _Atomic uint32_t atomic_uint32_t;
typedef _Atomic int64_t  atomic_int64_t;
typedef _Atomic uint64_t atomic_uint64_t;
#  if defined(__x86_64__)
typedef _Atomic int128_t  atomic_int128_t;
typedef _Atomic uint128_t atomic_uint128_t;
#  endif

typedef _Atomic intmax_t  atomic_intmax_t;
typedef _Atomic uintmax_t atomic_uintmax_t;

typedef _Atomic char16_t atomic_char16_t;
typedef _Atomic char32_t atomic_char32_t;
typedef _Atomic wchar_t  atomic_wchar_t;

typedef _Atomic intptr_t  atomic_intptr_t;
typedef _Atomic uintptr_t atomic_uintptr_t;
typedef _Atomic ssize_t   atomic_ssize_t;
typedef _Atomic size_t    atomic_size_t;
typedef _Atomic ptrdiff_t atomic_ptrdiff_t;

typedef _Atomic bool atomic_flag_t;

#endif

#if NO_STD || !defined(__cplusplus)
#  define atomic_thread_fence(mo)  __atomic_thread_fence(mo)
#  define atomic_signal_fence(mo)  __atomic_signal_fence(mo)
#  define atomic_is_lock_free(obj) __atomic_is_lock_free(sizeof(*(obj)), (obj))
#endif

//+ 我不知道为什么，但能用就行
#if NO_STD && !(defined(__cplusplus) && defined(__GNUC__) && !defined(__clang__))
#  define ATOMIC_FLAG_INIT false
#endif

//~ 自定义原子类型简称

#if defined(__cplusplus) && defined(__GNUC__) && !defined(__clang__)

#else

typedef atomic_bool   atom_bool;
typedef atomic_char   atom_char;
typedef atomic_schar  atom_schar;
typedef atomic_uchar  atom_uchar;
typedef atomic_short  atom_short;
typedef atomic_ushort atom_ushort;
typedef atomic_int    atom_int;
typedef atomic_uint   atom_uint;
typedef atomic_long   atom_long;
typedef atomic_ulong  atom_ulong;
typedef atomic_llong  atom_llong;
typedef atomic_ullong atom_ullong;

typedef atomic_int8_t   atom_i8;
typedef atomic_uint8_t  atom_u8;
typedef atomic_int16_t  atom_i16;
typedef atomic_uint16_t atom_u16;
typedef atomic_int32_t  atom_i32;
typedef atomic_uint32_t atom_u32;
typedef atomic_int64_t  atom_i64;
typedef atomic_uint64_t atom_u64;
#  if defined(__x86_64__)
typedef atomic_int128_t  atom_i128;
typedef atomic_uint128_t atom_u128;
#  endif

typedef atomic_intmax_t  atom_imax_t;
typedef atomic_uintmax_t atom_umax_t;

typedef atomic_char16_t atom_char16_t;
typedef atomic_char32_t atom_char32_t;
typedef atomic_wchar_t  atom_wchar_t;

typedef atomic_intptr_t  atom_iptr_t;
typedef atomic_uintptr_t atom_uptr_t;
typedef atomic_ssize_t   atom_ssize_t;
typedef atomic_size_t    atom_size_t;
typedef atomic_ptrdiff_t atom_pdiff_t;

typedef atomic_flag_t atom_flag_t;

#endif

#define atom_thread_fence(mo)  __atomic_thread_fence(mo)
#define atom_signal_fence(mo)  __atomic_signal_fence(mo)
#define atom_is_lock_free(obj) __atomic_is_lock_free(sizeof(*(obj)), (obj))

#define ATOM_FLAG_INIT false
