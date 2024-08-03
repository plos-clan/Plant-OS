#pragma once
#include "base.h"

#define BITMAP(_name_, _nbits_)                                                                    \
  struct {                                                                                         \
    const u32 length;                                                                              \
    const u32 nbits;                                                                               \
    u32       bits[(_nbits_ + 31) / 32];                                                           \
  } _name_ = {.length = (_nbits_ + 31) / 32, .nbits = _nbits_};

finline bool bitmap_get(u32 *map, i32 index) {
  u32 *bits = map + 2;
  return bits[index / 32] & ((u32)1 << (index % 32));
}
finline void bitmap_set(u32 *map, i32 index) {
  u32 *bits         = map + 2;
  bits[index / 32] |= ((u32)1 << (index % 32));
}
finline void bitmap_clear(u32 *map, i32 index) {
  u32 *bits         = map + 2;
  bits[index / 32] &= ~((u32)1 << (index % 32));
}
finline void bitmap_fillzero(u32 *map) {
  const u32 length = map[0];
  u32      *bits   = map + 2;
  for (i32 i = 0; i < length; i++) {
    bits[i] = 0;
  }
}
finline void bitmap_fillone(u32 *map) {
  const u32 length = map[0];
  u32      *bits   = map + 2;
  for (i32 i = 0; i < length; i++) {
    bits[i] = U32_MAX;
  }
}
finline i32 bitmap_first_zero(u32 *map) {
  const u32 length = map[0];
  const u32 nbits  = map[1];
  u32      *bits   = map + 2;
  for (i32 i = 0; i < length; i++) {
    if (!~bits[i]) continue;
    const i32 n = i * 32 + __builtin_ffs(~bits[i]) - 1;
    return n < nbits ? n : -1;
  }
  return -1;
}
finline i32 bitmap_first_one(u32 *map) {
  const u32 length = map[0];
  const u32 nbits  = map[1];
  u32      *bits   = map + 2;
  for (i32 i = 0; i < length; i++) {
    if (!bits[i]) continue;
    const i32 n = i * 32 + __builtin_ffs(bits[i]) - 1;
    return n < nbits ? n : -1;
  }
  return -1;
}
finline i32 bitmap_find_zeros(u32 *map, i32 count) {
  const u32 length = map[0];
  const u32 nbits  = map[1];
  u32      *bits   = map + 2;
  i32       start, zeros = 0;
  for (i32 i = 0; i < length; i++) {
    if (!~bits[i]) continue;
    for (i32 j = 0; j < 32; j++) {
      if ((bits[i] & (1 << j)) != 0) {
        zeros = 0;
        continue;
      }
      if (zeros == 0) start = i * 32 + j;
      zeros++;
      if (zeros >= count) return start;
    }
  }

  return -1;
}
finline i32 bitmap_find_ones(u32 *map, i32 count) {
  const u32 length = map[0];
  const u32 nbits  = map[1];
  u32      *bits   = map + 2;
  i32       start, ones = 0;
  for (i32 i = 0; i < length; i++) {
    if (!bits[i]) continue;
    for (i32 j = 0; j < 32; j++) {
      if ((bits[i] & (1 << j)) == 0) {
        ones = 0;
        continue;
      }
      if (ones == 0) start = i * 32 + j;
      ones++;
      if (ones >= count) return start;
    }
  }

  return -1;
}

#define bitmap_get(bitmap, index)        bitmap_get(&(bitmap), index)
#define bitmap_set(bitmap, index)        bitmap_set(&(bitmap), index)
#define bitmap_clear(bitmap, index)      bitmap_clear(&(bitmap), index)
#define bitmap_fillzero(bitmap)          bitmap_fillzero(&(bitmap))
#define bitmap_fillone(bitmap)           bitmap_fillone(&(bitmap))
#define bitmap_first_zero(bitmap)        bitmap_first_zero(&(bitmap))
#define bitmap_first_one(bitmap)         bitmap_first_one(&(bitmap))
#define bitmap_find_zeros(bitmap, count) bitmap_first_one(&(bitmap), count)
#define bitmap_find_ones(bitmap, count)  bitmap_first_one(&(bitmap), count)
