#pragma once
#include <define.h>

extern u32 memsize;

void memory_init(void *ptr, size_t size);
u32  memtest(u32 start, u32 end);
