// This code is released under the MIT License

#include <libc-base.h>

// str_builder

// 未完成，不用

typedef struct str_builder {
  char  *str;
  size_t len;
  size_t cap;
} *str_builder_t;

str_builder_t str_builder_new(size_t cap) {
  str_builder_t sb = malloc(sizeof(struct str_builder));
  sb->str          = malloc(cap);
  sb->len          = 0;
  sb->cap          = cap;
  return sb;
}

void str_builder_free(str_builder_t sb) {
  free(sb->str);
  free(sb);
}

void str_builder_append(str_builder_t sb, char ch) {
  if (sb->len >= sb->cap) {
    sb->cap *= 2;
    sb->str  = realloc(sb->str, sb->cap);
  }
  sb->str[sb->len++] = ch;
}

void str_builder_insert(str_builder_t sb, int pos, char ch) {
  if (sb->len >= sb->cap) {
    sb->cap *= 2;
    sb->str  = realloc(sb->str, sb->cap);
  }
  sb->len++;
}

void str_builder_delete(str_builder_t sb, int pos) {
  sb->len--;
}

void str_builder_clear(str_builder_t sb) {
  sb->len = 0;
}

void str_builder_str(str_builder_t sb, char *str) {
  strcpy(str, sb->str);
}
