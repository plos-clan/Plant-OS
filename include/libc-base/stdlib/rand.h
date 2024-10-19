#pragma once
#include <define.h>

int  rand();
void srand(uint seed);
int  rand_r(uint *_seed);

// MT19937 伪随机数生成器

typedef struct MT19937 {
  u32 mt[624];
  u32 idx;
} MT19937;

void MT19937_init(MT19937 *ctx, u32 seed);
void MT19937_generate(MT19937 *ctx);
u32  MT19937_extract(MT19937 *ctx);
