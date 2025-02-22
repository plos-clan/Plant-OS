#include <kernel.h>

#include "kernel-md5.h"
#include "kernel-mt19937.h"

#define RANDOM_SEED_POOL_LEN 64
static u32   random_seed_pool[RANDOM_SEED_POOL_LEN];
static usize random_seed_pool_p;

#define RANDOM_POOL_LEN 1024
static u32   random_pool[RANDOM_POOL_LEN];
static usize random_pool_h;
static usize random_pool_t;

__nif static void randrom_pool_put(u32 value) {
  with_no_interrupts({
    random_pool[random_pool_t++] = value;
    if (random_pool_t == RANDOM_POOL_LEN) random_pool_t = 0;
    if (random_pool_t == random_pool_h) {
      random_pool_h++;
      if (random_pool_h == RANDOM_POOL_LEN) random_pool_h = 0;
    }
  });
}

__nif static bool randrom_pool_get(u32 *value) {
  bool rets = false;
  with_no_interrupts({
    if (random_pool_h != random_pool_t) {
      *value = random_pool[random_pool_h++];
      if (random_pool_h == RANDOM_POOL_LEN) random_pool_h = 0;
      rets = true;
    }
  });
  return rets;
}

static u32 md5_h[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};

void ksrand(u32 value) {
  with_no_interrupts({
    random_seed_pool[random_seed_pool_p++] = value;
    if (random_seed_pool_p == RANDOM_SEED_POOL_LEN) {
      random_seed_pool_p = 0;
      for (int i = 0; i < RANDOM_SEED_POOL_LEN; i += 16) {
        MD5_iterate(md5_h, random_seed_pool + i);
      }
      for (int i = 0; i < 4; i++) {
        randrom_pool_put(md5_h[i]);
      }
    }
  });
}

static u32   mt19937[624];
static usize mt19937_p = 624;

static u32 randrom_get() {
  static u32 value;
  if (randrom_pool_get(&value)) return value;
  if (mt19937_p != 624) return k_MT19937_extract(mt19937, mt19937_p++);
  k_MT19937_init(mt19937, value);
  mt19937_p    = 1;
  return value = k_MT19937_extract(mt19937, 0);
}

byte krandb() {
  return (byte)(randrom_get() & 0xff);
}

i32 krand() {
  return (i32)(randrom_get() & (u32)INT_MAX);
}

u8 krand8() {
  return (u8)(randrom_get() & 0xff);
}

u16 krand16() {
  return (u16)(randrom_get() & 0xffff);
}

u32 krand32() {
  return randrom_get();
}

u64 krand64() {
  return ((u64)randrom_get() << 32) | randrom_get();
}
