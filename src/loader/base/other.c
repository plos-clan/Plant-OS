#include <dosldr.h>
void strtoupper(char *str) {
  while (*str != '\0') {
    if (*str >= 'a' && *str <= 'z') { *str -= 32; }
    str++;
  }
}
void clean(char *s, int len) {
  //清理某个内存区域（全部置0）
  int i;
  for (i = 0; i != len; i++) {
    s[i] = 0;
  }
  return;
}
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
