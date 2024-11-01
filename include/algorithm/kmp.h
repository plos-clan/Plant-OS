#pragma once
#include "base.h"

static ssize_t kmp_nsearch(cstr text, i32 textlen, cstr pattern, i32 patternlen) {
  i32 *lps = (i32 *)calloc(patternlen, 4);

  for (i32 i = 1, n = 0; i < patternlen; i++) {
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
      return i - j + 1;
    }
  }

  free(lps);
  return -1;
}

static ssize_t kmp_search(cstr text, cstr pattern) {
  return kmp_nsearch(text, strlen(text), pattern, strlen(pattern));
}

static void *kmp_nprepare(cstr pattern, i32 patternlen) {
  i32 *data = (i32 *)calloc(1 + PADDING_UP(patternlen, 4) / 4 + patternlen, 4);
  data[0]   = patternlen;
  memcpy(data + 1, pattern, patternlen);
  i32 *lps = data + 1 + PADDING_UP(patternlen, 4) / 4;

  for (i32 i = 1, n = 0; i < patternlen; i++) {
    if (pattern[i] == pattern[n]) {
      lps[i] = ++n;
    } else if (n != 0) {
      n = lps[n - 1];
      i--;
    }
  }

  return data;
}

static void *kmp_prepare(cstr pattern) {
  return kmp_nprepare(pattern, strlen(pattern));
}

static ssize_t kmp_nrun(cstr text, i32 textlen, const void *data) {
  i32  patternlen = *(const i32 *)data;
  cstr pattern    = (cstr)data + 4;
  var  lps        = (const i32 *)data + 1 + PADDING_UP(patternlen, 4) / 4;

  for (int i = 0, j = 0; i < textlen; i++) {
    if (pattern[j] == text[i]) {
      j++;
    } else if (j != 0) {
      j = lps[j - 1];
      i--;
    }
    if (j == patternlen) return i - j + 1;
  }

  return -1;
}

static ssize_t kmp_run(cstr text, void *data) {
  return kmp_nrun(text, strlen(text), data);
}
