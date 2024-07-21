/* a string lib for miniset */
#include "mstr.h"
#include <dosldr.h>
mstr *mstr_init() {
  mstr *ms = (mstr *)malloc(sizeof(mstr));
  if (!ms) { return NULL; }
  ms->buf = (char *)malloc(sizeof(char));
  if (!ms->buf) {
    free(ms);
    return NULL;
  }
  ms->len         = 0;
  ms->malloc_size = sizeof(char);
  ms->buf[0]      = '\0';
  return ms;
}

void mstr_add_str(mstr *ms, char *str) {
  size_t str_len = strlen(str);
  if (ms->len + str_len + 1 > ms->malloc_size) {
    while (ms->malloc_size <= ms->len + str_len)
      ms->malloc_size *= 2;
    ms->buf = (char *)realloc(ms->buf, ms->malloc_size * sizeof(char));
    if (!ms->buf) { return; }
  }
  strcat(ms->buf, str);
  ms->len += str_len;
}

void mstr_add_char(mstr *ms, char c) {
  if (ms->len + 2 > ms->malloc_size) {
    ms->malloc_size *= 2;
    ms->buf          = (char *)realloc(ms->buf, ms->malloc_size * sizeof(char));
    if (!ms->buf) { return; }
  }
  ms->buf[ms->len]     = c;
  ms->buf[ms->len + 1] = '\0';
  ms->len++;
}
void mstr_backspace(mstr *ms) {
  ms->len--;
  ms->buf[ms->len] = 0;
}
char *mstr_get(mstr *ms) {
  return ms->buf;
}

void mstr_free(mstr *ms) {
  free(ms->buf);
  free(ms);
}
