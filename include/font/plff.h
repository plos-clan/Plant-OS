#pragma once
#include <data-structure.h>
#include <libc-base.h>

#define PLFF_MAGIC MAGIC32('p', 'l', 'f', 'f')

typedef struct __PACKED__ plff_file_char {
  i32 code;    // 字符的 unicode 码点
  u32 img;     // 字符的位图
  i16 top;     // 到上方的距离
  i16 left;    // 到左侧的距离
  i16 width;   // 位图的宽度
  i16 height;  // 位图的高度
  i16 advance; // 字符的宽度
} *plff_file_char_t;

struct __ALIGN4__ plff_file {
  u32                   magic;          //
  u32                   nchars;         //
  u16                   height;         //
  u16                   channels;       //
  u32                   img_size;       //
  u32                   img_zippedsize; //
  struct plff_file_char chars[];        //
};
