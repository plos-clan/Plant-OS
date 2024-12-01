#pragma once
#include <define.h>

#include "../libc-base/stdlib/alloc.h"
#include "../libc-base/string/alloc.h"
#include "../libc-base/string/mem.h"
#include "../libc-base/string/str.h"
#include "../libc-base/thread/spin.h"

dlimport int printf(cstr _rest fmt, ...) __attr(format(printf, 1, 2));
