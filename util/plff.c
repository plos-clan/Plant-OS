#include <fcntl.h>
#include <freetype/freetype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <define/config.h>
#undef NO_STD
#include <font.h>

FT_Library library;
FT_Face    face;

#if 1
int main() {
  FT_Init_FreeType(&library);
  FT_New_Face(library, "SourceHanSans-Light.ttc", 2, &face);
  // FT_New_Face(library, "Ligconsolata-Regular.ttf", 0, &face);
  int pixel_height = 14;
  FT_Set_Pixel_Sizes(face, 0, pixel_height);

  // 遍历字符集的Unicode码点范围
  FT_ULong charcode;
  FT_UInt  glyph_index;

  charcode = FT_Get_First_Char(face, &glyph_index);

#  define MAX_CHARS 1000000

  font_char_t data = malloc(MAX_CHARS * sizeof(*data));

  int index = 0;

  uint8_t *image = malloc(MAX_CHARS * 1200);
  uint8_t *img   = image;

  while (glyph_index != 0) {
    // 加载字符
    FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

    // 渲染字符的位图
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD);
    FT_GlyphSlot glyph_slot = face->glyph;

    int width  = glyph_slot->bitmap.width / 3;
    int height = glyph_slot->bitmap.rows;

    data[index] = (struct font_char){
        .code    = charcode,
        .img     = img,
        .top     = pixel_height - glyph_slot->bitmap_top,
        .left    = glyph_slot->bitmap_left,
        .width   = width,
        .height  = height,
        .advance = glyph_slot->advance.x >> 6,
    };
    font_char_t c = &data[index];

    int pitch = glyph_slot->bitmap.pitch;

    // 将字符位图数据复制到位图中
    uint8_t *d = glyph_slot->bitmap.buffer;
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        *img++ = d[y * pitch + x * 3];
        *img++ = d[y * pitch + x * 3 + 1];
        *img++ = d[y * pitch + x * 3 + 2];
      }
    }

    // 获取下一个字符的Unicode码点和索引
    charcode = FT_Get_Next_Char(face, charcode, &glyph_index);
    index++;
  }

  FT_Done_Face(face);
  FT_Done_FreeType(library);

  struct font font = {
      .img        = image,
      .height     = pixel_height,
      .nchars     = index,
      .channels   = 3,
      .chars_list = data,
  };

  size_t size;
  void  *font_data = font_save_to_mem(&font, &size);

  FILE *file = fopen("font.plff", "wb");
  fwrite(font_data, 1, size, file);
  fclose(file);

  return 0;
}
#endif

#if 0
int main() {
  FT_Init_FreeType(&library);
  FT_New_Face(library, "SourceHanSans-Light.ttc", 2, &face);
  // FT_New_Face(library, "Ligconsolata-Regular.ttf", 0, &face);
  int pixel_height = 14;
  FT_Set_Pixel_Sizes(face, 0, pixel_height);

  // 遍历字符集的Unicode码点范围
  FT_ULong charcode;
  FT_UInt  glyph_index;

  charcode = FT_Get_First_Char(face, &glyph_index);

  FILE *file = fopen("font.bin", "wb");

#  define MAX_CHARS 1000000

  font_char_t data = malloc(MAX_CHARS * sizeof(*data));

  u32 index = 0;

  uint8_t *image = malloc(MAX_CHARS * 1200);
  uint8_t *img   = image;

  while (glyph_index != 0) {
    // 加载字符
    FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

    // 渲染字符的位图
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD);
    FT_GlyphSlot glyph_slot = face->glyph;

    int width  = glyph_slot->bitmap.width / 3;
    int height = glyph_slot->bitmap.rows;
    int pitch  = glyph_slot->bitmap.pitch;

    // *(u32 *)img  = charcode;
    // img         += 4;

    for (int y = 0; y < 16; y++) {
      for (int x = 0; x < 16; x++) {
        int cx = x - glyph_slot->bitmap_left, cy = y - (pixel_height - glyph_slot->bitmap_top);
        if (cx >= 0 && cx < width && cy >= 0 && cy < height) {
          *img++ = glyph_slot->bitmap.buffer[cy * pitch + cx * 3];
          *img++ = glyph_slot->bitmap.buffer[cy * pitch + cx * 3 + 1];
          *img++ = glyph_slot->bitmap.buffer[cy * pitch + cx * 3 + 2];
        } else {
          *img++ = 0;
          *img++ = 0;
          *img++ = 0;
        }
      }
    }

    // 获取下一个字符的Unicode码点和索引
    charcode = FT_Get_Next_Char(face, charcode, &glyph_index);
    index++;
  }

  FT_Done_Face(face);
  FT_Done_FreeType(library);

  // fwrite(&index, 1, 4, file);
  fwrite(image, 1, img - image, file);
  fclose(file);

  return 0;
}
#endif
