#pragma once
#include <data-structure.h>
#include <libc-base.h>

#define FONT_IMG_WIDTH 1024

typedef struct font_char {
  int32_t code : 24; // 字符的unicode码点
  int8_t  top;       //
  int8_t  left;      //
  int8_t  width;     //
  int8_t  height;    //
  int8_t  advance;   //
  union {
    int32_t offset;
    byte   *img; //
  };
} *font_char_t;

typedef struct font {
  int32_t     rc;
  byte       *img;
  int32_t     img_size;
  int32_t     channels;
  int32_t     height;
  int32_t     nchars;
  font_char_t chars_list;
  rbtree_t    chars;
} *font_t;

extern font_t font_load(const char *filename);
extern int    font_save(const char *filename, font_t font);
extern void   font_free(font_t font);

extern font_t font_ref(font_t font);
extern void   font_unref(font_t font);

finline font_char_t font_getchar(font_t font, int32_t code) {
  return rbtree_get(font->chars, code);
}
