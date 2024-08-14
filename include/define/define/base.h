// This code is released under the MIT License

#pragma once
#include "00-include.h"

// 获取数组的长度
#ifndef lengthof
#  define lengthof(arr) (sizeof(arr) / sizeof(*(arr)))
#endif

// 获取表达式的类型，类似于 auto
#define typeof(arg) __typeof__((void)0, arg)

#define alloca(size) __builtin_alloca(size)

#if NO_BUILTIN
#  define __has(name) (0)
#else
#  define __has(name) (__has_builtin(__builtin_##name))
#endif

#define CONCAT_(a, b) a##b
#define CONCAT(a, b)  CONCAT_(a, b)

#if STD_SAFE_API
#  define __std_safe__(code) ((void)({code}))
#else
#  define __std_safe__(code) ((void)(0))
#endif

#if SAFE_API
#  define __safe__(code) ((void)({code}))
#else
#  define __safe__(code) ((void)(0))
#endif

#define __ARGN_PRIVATE__(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15,     \
                         _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29,     \
                         _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43,     \
                         _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57,     \
                         _58, _59, _60, _61, _62, _63, _64, n, ...)                                \
  n
#define __ARGN__(...)                                                                              \
  __ARGN_PRIVATE__(0, ##__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50,   \
                   49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
                   29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
                   9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define PADDING_DOWN(size, to) ((size_t)(size) / (size_t)(to) * (size_t)(to))
#define PADDING_UP(size, to)   PADDING_DOWN((size_t)(size) + (size_t)(to) - (size_t)1, to)

#define infinite_loop while (true)
