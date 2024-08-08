#pragma once
#include <define.h>

dlimport int vsprintf(char *_rest s, cstr _rest fmt, va_list va);

dlimport int sprintf(char *_rest s, cstr _rest fmt, ...);

dlimport int vsnprintf(char *_rest s, size_t maxlen, cstr _rest fmt, va_list va);

dlimport int printf(cstr _rest fmt, ...);
