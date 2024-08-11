#include <loader.h>

void insert_char(char *str, int pos, char ch) {
  int i;
  for (i = strlen(str); i >= pos; i--) {
    str[i + 1] = str[i];
  }
  str[pos] = ch;
}
void delete_char(char *str, int pos) {
  int i;
  for (i = pos; i < strlen(str); i++) {
    str[i] = str[i + 1];
  }
}
void *realloc(void *ptr, u32 size) {
  void *new = malloc(size);
  if (ptr) {
    memcpy(new, ptr, *(int *)((int)ptr - 4));
    free(ptr);
  }
  return new;
}
