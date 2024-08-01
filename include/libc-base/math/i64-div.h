#pragma once
#include "base.h"

// 在 32 位下支持 64 位整数除法
#ifdef __i686__
dlimport void i64_div(i64 a, i64 b, i64 *_c, i64 *_d);
dlimport void u64_div(u64 a, u64 b, u64 *_c, u64 *_d);
#endif
