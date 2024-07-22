#pragma once
#include <define.h>
#include <type.h>

#define BITMAP(name, nbits) u32 name[(nbits + 31) / 32]

finline void bitmap_set(u32 *bitmap, int32_t index) {
  bitmap[index / 32] |= ((u32)1 << (index % 32));
}
finline void bitmap_clear(u32 *bitmap, int32_t index) {
  bitmap[index / 32] &= ~((u32)1 << (index % 32));
}
finline void bitmap_clearall(u32 *bitmap, int32_t nbits) {
  for (int32_t i = 0; i < (nbits + 31) / 32; i++) {
    bitmap[i] = 0;
  }
}
finline bool bitmap_bit(u32 *bitmap, int32_t index) {
  return bitmap[index / 32] & ((u32)1 << (index % 32));
}
