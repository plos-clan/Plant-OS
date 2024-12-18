// This code is released under the MIT License

#pragma once

#pragma GCC system_header

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 是否无需上锁

//- 参考 gcc   /usr/lib/gcc/x86_64-linux-gnu/14/include/stdatomic.h
//- 参考 clang /usr/lib/llvm-19/lib/clang/19/include/stdatomic.h
//-   注意 clang 兼容 gcc 标准头文件，所以我们可以直接参照 gcc 的实现

#define ATOMIC_BOOL_LOCK_FREE     __GCC_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE     __GCC_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE __GCC_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE __GCC_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE  __GCC_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE    __GCC_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE      __GCC_ATOMIC_INT_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE     __GCC_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE    __GCC_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE  __GCC_ATOMIC_POINTER_LOCK_FREE

//+ c32 新增的 char8 类型

#if __STDC_VERSION__ >= 202311L
#  define ATOMIC_CHAR8_T_LOCK_FREE __CLANG_ATOMIC_CHAR8_T_LOCK_FREE
#endif
