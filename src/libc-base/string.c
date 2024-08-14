// This code is released under the MIT License

#include <libc-base.h>

u32 utf8_to_32(cstr *s_p) {
  auto s = (const u8 *)*s_p;
  u32  code;
  if (s[0] < 0x80) { // ASCII字符（单字节）
    code = *s++;
  } else if ((s[0] & 0xe0) == 0xc0) { // 2字节UTF-8字符
    if ((s[1] & 0xc0) != 0x80) goto err;
    code  = (*s++ & 0x1f) << 6;
    code |= (*s++ & 0x3f);
  } else if ((s[0] & 0xf0) == 0xe0) { // 3字节UTF-8字符
    if ((s[1] & 0xc0) != 0x80 || (s[2] & 0xc0) != 0x80) goto err;
    code  = (*s++ & 0x0F) << 12;
    code |= (*s++ & 0x3f) << 6;
    code |= (*s++ & 0x3f);
  } else if ((s[0] & 0xf8) == 0xf0) { // 4字节UTF-8字符
    if ((s[1] & 0xc0) != 0x80 || (s[2] & 0xc0) != 0x80 || (s[3] & 0xc0) != 0x80) goto err;
    code  = (*s++ & 0x07) << 18;
    code |= (*s++ & 0x3f) << 12;
    code |= (*s++ & 0x3f) << 6;
    code |= (*s++ & 0x3f);
  } else { // 非法的UTF-8序列
    goto err;
  }
  *s_p = (cstr)s;
  return code;

err:
  s++;
  *s_p = (cstr)s;
  return 0xfffd; // 跳过非法序列
}

void utf8_to_32s(u32 *d, cstr s) {
  while (*s) {
    *d++ = utf8_to_32(&s);
  }
  *d++ = 0;
}

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
