#pragma once
#include "../crypto/hash.h"
#include "../stdio/print.h"
#include "../stdlib/alloc.h"
#include "mem.h"
#include "str.h"

#if NO_STD

finline char *strdup(cstr s) noexcept {
  size_t len = strlen(s);
  auto   ptr = (char *)malloc(len + 1);
  if (ptr == null) return null;
  memcpy(ptr, s, len + 1);
  return ptr;
}

finline char *strndup(cstr s, size_t n) noexcept {
  size_t len = strlen(s);
  if (n > len) n = len;
  auto ptr = (char *)malloc(n + 1);
  if (ptr == null) return null;
  memcpy(ptr, s, n);
  ptr[n] = '\0';
  return ptr;
}

#endif

finline void *memdup(const void *s, size_t n) {
  void *ptr = malloc(n);
  if (ptr == null) return null;
  memcpy(ptr, s, n);
  return ptr;
}

// 将 C 标准的字符串 转换为 xstr
finline xstr c2xstr(cstr s) {
  if (s == null) return null;
  size_t len = strlen(s);
  xstr   x   = (xstr)malloc(sizeof(struct _xstr) + len + 1);
  if (x == null) return null;
  x->len  = len;
  x->hash = memhash(s, len);
  memcpy(x->data, s, len + 1);
  return x;
}

// 将 xstr 转换为 C 标准的字符串，并删除 xstr
finline char *x2str(xstr s) {
  if (s == null) return null;
  auto ptr = strdup(s->data);
  free(s);
  return ptr;
}

finline xstr xstrdup(xstr s) {
  if (s == null) return null;
  xstr x = (xstr)malloc(sizeof(struct _xstr) + s->len + 1);
  if (x == null) return null;
  x->len  = s->len;
  x->hash = s->hash;
  memcpy(x->data, s->data, s->len + 1);
  return x;
}

finline char *pathcat(cstr p1, cstr p2) {
  char *p = (char *)malloc(strlen(p1) + strlen(p2) + 2);
  if (p1[strlen(p1) - 1] == '/') {
    sprintf(p, "%s%s", p1, p2);
  } else {
    sprintf(p, "%s/%s", p1, p2);
  }
  return p;
}
