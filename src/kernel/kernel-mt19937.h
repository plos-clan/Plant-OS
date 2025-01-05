#pragma once
#include <define.h>

static void k_MT19937_init(u32 mt[624], u32 seed) {
  mt[0] = seed;
  for (u32 i = 1; i < 624; i++) {
    u32 n = mt[i - 1] ^ (mt[i - 1] >> 30);
    mt[i] = 0x6c078965 * n + i;
  }
}

static u32 k_MT19937_extract(const u32 mt[624], usize idx) {
  u32 y  = mt[idx];
  y     ^= (y >> 11);
  y     ^= (y << 7) & 0x9d2c5680;
  y     ^= (y << 15) & 0xefc60000;
  y     ^= (y >> 18);
  return y;
}