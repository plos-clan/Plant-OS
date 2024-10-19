#pragma once
#include <define.h>

extern u32 total_mem_size;

void memory_init(void *ptr, size_t size);
u32  memtest(u32 start, u32 end);
