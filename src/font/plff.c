#include <font.h>

void *plff_encode_rgb(const void *_data, size_t img_size, size_t *size_p);
void *plff_encode_rgba(const void *_data, size_t img_size, size_t *size_p);
void *plff_decode_rgb(const void *data, size_t size, size_t img_size);
void *plff_decode_rgba(const void *data, size_t size, size_t img_size);

dlexport plff_t plff_load_from_mem(const void *data, size_t size) {
  if (data == null || size < sizeof(struct plff_file)) return null;

  const struct plff_file *info = data;
  if (info->magic != PLFF_MAGIC) return null;

  plff_t font = malloc(sizeof(*font));
  if (font == null) return null;

  font->nchars   = info->nchars;
  font->height   = info->height;
  font->channels = info->channels;

  font->chars_list = malloc(info->nchars * sizeof(struct font_char));
  if (font->chars_list == null) {
    free(font);
    return null;
  }

  const void *img_data = &info->chars[info->nchars];
  if (info->channels == 4) {
    font->img = plff_decode_rgba(img_data, info->img_zippedsize, info->img_size / 4);
  } else {
    font->img = plff_decode_rgb(img_data, info->img_zippedsize, (info->img_size + 2) / 3);
  }
  printf("img_data: %p\n", img_data);
  printf("font->img: %p\n", font->img);
  if (font->img == null) {
    free(font->chars_list);
    free(font);
    return null;
  }

  font->chars = null;
  for (int32_t i = 0; i < font->nchars; i++) {
    auto s     = &info->chars[i];
    auto c     = &font->chars_list[i];
    c->code    = s->code;
    c->img     = font->img + s->img;
    c->top     = s->top;
    c->left    = s->left;
    c->width   = s->width;
    c->height  = s->height;
    c->advance = s->advance;
    rbtree_insert(font->chars, c->code, c);
  }

  return font;
}

dlexport void *font_save_to_mem(plff_t font, size_t *size_p) {
  if (font == null || size_p == null) return null;

  const auto last_char = font->chars_list[font->nchars - 1];
  size_t img_size = last_char.img + last_char.height * last_char.width * font->channels - font->img;

  size_t img_zippedsize;
  void  *data = null;
  if (font->channels == 4) {
    data = plff_encode_rgba(font->img, img_size / 4, &img_zippedsize);
  } else {
    data = plff_encode_rgb(font->img, (img_size + 2) / 3, &img_zippedsize);
  }
  if (data == null) return null;

  size_t info_size =
      sizeof(struct plff_file) + font->nchars * sizeof(struct plff_file_char) + img_zippedsize;
  struct plff_file *info = malloc(info_size);
  if (info == null) {
    free(data);
    return null;
  }

  info->magic          = PLFF_MAGIC;
  info->nchars         = font->nchars;
  info->height         = font->height;
  info->channels       = font->channels;
  info->img_size       = img_size;
  info->img_zippedsize = img_zippedsize;

  void *img_data = &info->chars[info->nchars];
  memcpy(img_data, data, img_zippedsize);
  free(data);

  for (int32_t i = 0; i < font->nchars; i++) {
    font_char_t      s = &font->chars_list[i];
    plff_file_char_t c = &info->chars[i];
    c->code            = s->code;
    c->img             = s->img - font->img;
    c->top             = s->top;
    c->left            = s->left;
    c->width           = s->width;
    c->height          = s->height;
    c->advance         = s->advance;
  }

  *size_p = info_size;
  return info;
}

dlexport void font_free(plff_t font) {
  if (font == null) return;
  free(font->img);
  free(font->chars_list);
  rbtree_free(font->chars);
  free(font);
}
