// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "00-include.h"

#undef va_start
#undef va_end
#undef va_arg
#undef va_copy

typedef __builtin_va_list va_list;
#define va_start(ap, para) __builtin_va_start(ap, para)
#define va_end(ap)         __builtin_va_end(ap)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_copy(dest, src) __builtin_va_copy(dest, src)
