#pragma once
#include "mem.h"
#include "str.h"

finline char *pathcat(char *p1, cstr p2) {
  val len = strlen(p1);
  if (p1[len - 1] == '/') {
    strcpy(p1 + len, p2);
  } else {
    p1[len] = '/';
    strcpy(p1 + len + 1, p2);
  }
  return p1;
}
