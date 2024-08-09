#pragma once
#include "00-include.h"

#define MASK8(n)  ((u8)1 << (n))
#define MASK16(n) ((u16)1 << (n))
#define MASK32(n) ((u32)1 << (n))
#define MASK64(n) ((u64)1 << (n))
#define MASK(n)   ((size_t)1 << (n))

#define MAGIC16(a, b)                   ((u16)(a) | ((u16)(b) << 8))
#define MAGIC32(a, b, c, d)             ((u32)(a) | ((u32)(b) << 8) | ((u32)(c) << 16) | ((u32)(d) << 24))
#define MAGIC64(a, b, c, d, e, f, g, h) (MAGIC32(a, b, c, d) | ((u64)MAGIC32(e, f, g, h) << 32))
