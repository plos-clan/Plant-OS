#pragma once
#include "../stdlib/alloc.h"
#include "mem.h"
#include "str.h"

finline char *strdup(cstr _s) {
  size_t len = strlen(_s);
  auto   ptr = (char *)malloc(len + 1);
  if (ptr == null) return null;
  memcpy(ptr, _s, len + 1);
  return ptr;
}

finline char *strndup(cstr _s, size_t _n) {
  size_t len = strlen(_s);
  if (_n > len) _n = len;
  auto ptr = (char *)malloc(_n + 1);
  if (ptr == null) return null;
  memcpy(ptr, _s, _n);
  ptr[_n] = '\0';
  return ptr;
}

finline void *memdup(const void *_s, size_t _n) {
  void *ptr = malloc(_n);
  if (ptr == null) return null;
  memcpy(ptr, _s, _n);
  return ptr;
}
