#pragma once
#include "base.h"

static ssize_t bm_nsearch(cstr text, i32 textlen, cstr pattern, i32 patternlen) {
  i32 bc[256];
  for (i32 i = 0; i < 256; i++) {
    bc[i] = patternlen;
  }
  for (i32 i = 0; i < patternlen - 1; i++) {
    bc[(byte)pattern[i]] = patternlen - i - 1;
  }

  for (i32 i = patternlen - 1; i < textlen;) {
    for (i32 j = patternlen - 1; text[i] == pattern[j]; i--, j--) {
      if (j == 0) return i;
    }
    i += bc[(byte)text[i]];
  }

  return -1;
}

static ssize_t bm_search(cstr text, cstr pattern) {
  return bm_nsearch(text, strlen(text), pattern, strlen(pattern));
}

static void *bm_nprepare(cstr pattern, i32 patternlen) {
  i32 *data = (i32 *)calloc(1 + PADDING_UP(patternlen, 4) / 4 + 256, 4);
  data[0]   = patternlen;
  memcpy(data + 1, pattern, patternlen);
  i32 *bc = data + 1 + PADDING_UP(patternlen, 4) / 4;

  for (i32 i = 0; i < 256; i++) {
    bc[i] = patternlen;
  }
  for (i32 i = 0; i < patternlen - 1; i++) {
    bc[(byte)pattern[i]] = patternlen - i - 1;
  }

  return data;
}

static void *bm_prepare(cstr pattern) {
  return bm_nprepare(pattern, strlen(pattern));
}

static ssize_t bm_run(cstr text, i32 textlen, const void *data) {
  i32  patternlen = *(const i32 *)data;
  cstr pattern    = (cstr)data + 4;
  var  bc         = (const i32 *)data + 1 + PADDING_UP(patternlen, 4) / 4;

  for (i32 i = patternlen - 1; i < textlen;) {
    for (i32 j = patternlen - 1; text[i] == pattern[j]; i--, j--) {
      if (j == 0) return i;
    }
    i += bc[(byte)text[i]];
  }

  return -1;
}
