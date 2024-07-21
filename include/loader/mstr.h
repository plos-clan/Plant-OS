#pragma once
#include <stddef.h>
typedef struct {
  char  *buf;         // save the data of the string
  size_t len;         // the length of the string
  size_t malloc_size; // when does the buf of the data need to remalloc?
} mstr;

mstr *mstr_init();
void  mstr_add_str(mstr *ms, char *str);
void  mstr_add_char(mstr *ms, char c);
char *mstr_get(mstr *ms);
void  mstr_free(mstr *ms);
void  mstr_backspace(mstr *ms);
