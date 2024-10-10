// This code is released under the MIT License

#pragma once
#include "00-include.h"

#ifdef __cplusplus
static const auto null = nullptr;
#else
#  define null ((void *)0)
#endif

#define NULL 0

// gcc 扩展，让 C 中的 auto 具有和 C++ 类似的语义
// 神经你 C23 加这鬼玩意干嘛，没活可以咬个打火机
#if !defined(__cplusplus) && __STDC_VERSION__ < 202300L
#  define auto __auto_type
#endif

#define var auto

typedef __INT8_TYPE__  sbyte;
typedef __UINT8_TYPE__ byte;

typedef int errno_t;

typedef __INT64_TYPE__ time_t;
