#pragma once
#include <define.h>

dlimport int vsprintf(char *_rest s, cstr _rest fmt, va_list va) __THROWNL;

dlimport int sprintf(char *_rest s, cstr _rest fmt, ...) __THROWNL;

dlimport int vsnprintf(char *_rest s, size_t maxlen, cstr _rest fmt, va_list va)
    __THROWNL __attr(format(printf, 3, 0));

dlimport int printf(cstr _rest fmt, ...);
