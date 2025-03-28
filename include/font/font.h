#pragma once
#include <data-structure.h>
#include <libc-base.h>

typedef struct font_char {
  i32   code;    // 字符的 unicode 码点
  byte *img;     // 字符的位图
  i16   top;     // 到上方的距离
  i16   left;    // 到左侧的距离
  i16   width;   // 位图的宽度
  i16   height;  // 位图的高度
  i16   advance; // 字符的宽度
} *font_char_t;

typedef struct font {
  u32         nchars;   //
  u16         height;   // 标准字符高度 (大小)
  u16         channels; // 单通道 或 RGBA
  font_char_t chars_list;
  byte       *img;
  rbtree_t    chars;
} *plff_t;

// plff_t font_alloc() {
//   plff_t font = malloc(sizeof(struct font));
//   if (font == null) return null;
//   font->nchars   = 0;
//   font->height   = 0;
//   font->channels = 0;
//   font->chars    = null;
//   return font;
// }

plff_t font_load(const char *filename);
int    font_save(const char *filename, plff_t font);
void   font_free(plff_t font);

plff_t font_ref(plff_t font);
void   font_unref(plff_t font);

plff_t plff_load_from_mem(const void *data, size_t size);
void  *font_save_to_mem(plff_t font, size_t *size_p);

finline font_char_t font_getchar(plff_t font, u32 code) {
  return rbtree_get(font->chars, (i32)code);
}
