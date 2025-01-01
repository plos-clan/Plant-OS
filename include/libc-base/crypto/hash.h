#pragma once
#include <define.h>

inline_pure u32 memhash(const void *_rest src, size_t len) {
  if (!src) return 0;
  u32 h = 0;
  for (size_t i = 0; i < len; i++)
    h = h * 131 + ((const byte *)src)[i];
  return h;
}

inline_pure u32 strnhash(const void *_rest src, size_t len) {
  if (!src) return 0;
  u32 h = 0;
  for (size_t i = 0; ((const byte *)src)[i] && i < len; i++)
    h = h * 131 + ((const byte *)src)[i];
  return h;
}

inline_pure u32 strhash(cstr _rest src) {
  if (!src) return 0;
  u32 h = 0;
  for (; *src; src++)
    h = h * 131 + *(const byte *)src;
  return h;
}

inline_const u32 i8hash(i8 value) {
  return (u32)value * 2491757;
}
inline_const u32 u8hash(u8 value) {
  return value * 2491757;
}
inline_const u32 i16hash(i16 value) {
  return (u32)value * 2491757;
}
inline_const u32 u16hash(u16 value) {
  return value * 2491757;
}
inline_const u32 i32hash(i32 value) {
  return (u32)value * 2491757;
}
inline_const u32 u32hash(u32 value) {
  return value * 2491757;
}
inline_const u32 i64hash(i64 value) {
  return ((u32)value * 2491757) ^ (((u64)value >> 32) * 491747);
}
inline_const u32 u64hash(u64 value) {
  return (value * 2491757) ^ ((value >> 32) * 491747);
}
