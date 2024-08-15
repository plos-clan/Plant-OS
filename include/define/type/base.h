// This code is released under the MIT License

#pragma once
#include "00-include.h"

#ifdef __cplusplus
static const auto null = nullptr;
#else
#  define null 0
#endif

#define NULL 0

// gcc 扩展，让 C 中的 auto 具有和 C++ 类似的语义
#ifndef __cplusplus
#  define auto __auto_type
#endif

typedef __INT8_TYPE__  sbyte;
typedef __UINT8_TYPE__ byte;

typedef int errno_t;
