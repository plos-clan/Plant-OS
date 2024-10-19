#pragma once
#include <define.h>

u32   memtest(u32 start, u32 end);
void  memman_init(void *ptr, size_t size);
void *page_alloc(int size);
void  page_free(void *p, int size);
