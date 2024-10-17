#pragma once
#include <define.h>

finline void insert_char(char *str, size_t pos, char ch) {
  char tmp1 = str[pos];
  str[pos]  = ch;
  for (size_t i = pos + 1;; i++) {
    char tmp2 = str[i];
    str[i]    = tmp1;
    tmp1      = tmp2;
    if (tmp1 == '\0') break;
  }
}

finline void delete_char(char *str, size_t pos) {
  for (size_t i = pos;; i++) {
    str[i] = str[i + 1];
    if (str[i] == '\0') break;
  }
}
