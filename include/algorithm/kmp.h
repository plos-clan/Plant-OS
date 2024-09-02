#pragma once
#include "base.h"

ssize_t kmp_search(cstr text, cstr pattern) {
  i32  textlen    = strlen(text);
  i32  patternlen = strlen(pattern);
  i32 *lps        = (i32 *)calloc(patternlen, 4);

  for (i32 i = 1, n = 0; pattern[i] != '\0'; i++) {
    if (pattern[i] == pattern[n]) {
      lps[i] = ++n;
    } else if (n != 0) {
      n = lps[n - 1];
      i--;
    }
  }

  for (i32 i = 0, j = 0; i < textlen; i++) {
    if (pattern[j] == text[i]) {
      j++;
    } else if (j != 0) {
      j = lps[j - 1];
      i--;
    }
    if (j == patternlen) {
      free(lps);
      return i - j;
    }
  }

  free(lps);
  return -1;
}

void *kmp_prepare(cstr pattern) {
  i32  patternlen = strlen(pattern);
  i32 *data       = (i32 *)malloc(1 + PADDING_UP(patternlen, 4) + patternlen * 4);
  data[0]         = patternlen;
  memcpy(data + 1, pattern, patternlen);
  i32 *lps = data + 1 + PADDING_UP(patternlen, 4) / 4;

  for (i32 i = 1, n = 0; pattern[i] != '\0'; i++) {
    if (pattern[i] == pattern[n]) {
      lps[i] = ++n;
    } else if (n != 0) {
      n = lps[n - 1];
      i--;
    }
  }

  return lps - 1;
}

ssize_t kmp_run(cstr text, void *data) {
  i32  textlen    = strlen(text);
  i32  patternlen = *(i32 *)data;
  cstr pattern    = (cstr)data + 4;
  i32 *lps        = (i32 *)data + 1 + PADDING_UP(patternlen, 4) / 4;

  for (int i = 0, j = 0; i < textlen; i++) {
    if (pattern[j] == text[i]) {
      j++;
    } else if (j != 0) {
      j = lps[j - 1];
      i--;
    }
    if (j == patternlen) return i - j;
  }

  return -1;
}
