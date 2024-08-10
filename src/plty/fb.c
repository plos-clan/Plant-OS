#include <plty.h>

// ----------------------------------------------------------------------------------------------------

static const color_t color_table_16[] = {
    {0,   0,   0,   255},
    {0,   0,   128, 255},
    {0,   128, 0,   255},
    {0,   128, 128, 255},
    {128, 0,   0,   255},
    {128, 0,   128, 255},
    {128, 128, 0,   255},
    {192, 192, 192, 255},
    {128, 128, 128, 255},
    {0,   0,   255, 255},
    {0,   255, 0,   255},
    {0,   255, 255, 255},
    {255, 0,   0,   255},
    {255, 0,   255, 255},
    {255, 255, 0,   255},
    {255, 255, 255, 255},
};

finline int color_diff(color_t c1, color_t c2) {
  int d1 = c1.r - c2.r;
  int d2 = c1.g - c2.g;
  int d3 = c1.b - c2.b;
  return d1 * d1 + d2 * d2 + d3 * d3;
}

static int color_best_match(color_t c) {
  int best_id  = 0;
  int diff_min = color_diff(c, color_table_16[0]);
#pragma unroll
  for (int i = 1; i < lengthof(color_table_16); i++) {
    int diff = color_diff(c, color_table_16[i]);
    if (diff < diff_min) {
      best_id  = i;
      diff_min = diff;
    }
  }
  return best_id;
}

finline void plty_fb_clear(plty_fb *fb) {
  for (size_t i = 0; i < fb->width * fb->height; i++) {
    fb->vbuf[i] = ' ';
  }
  fb->len       = -1;
  fb->cur_x     = 0;
  fb->cur_y     = 0;
  fb->cur_color = (tty_light_gray << 8) | (tty_black << 12);
}

finline int plty_fb_getcolor(plty_fb *fb, int x, int y) {
  return fb->vbuf[y * fb->width + x] >> 8;
}

finline void plty_fb_setcolor(plty_fb *fb, int fg, int bg) {
  fb->cur_color = ((u8)(fg & 15) << 8) | ((u8)(bg & 15) << 12);
}

finline int plty_fb_getfg(plty_fb *fb, int x, int y) {
  return (fb->vbuf[y * fb->width + x] >> 8) & 15;
}

finline int plty_fb_getbg(plty_fb *fb, int x, int y) {
  return fb->vbuf[y * fb->width + x] >> 12;
}

finline int plty_fb_getch(plty_fb *fb, int x, int y) {
  return fb->vbuf[y * fb->width + x] & 0xff;
}

finline void plty_fb_setch(plty_fb *fb, int x, int y, int c) {
  fb->vbuf[y * fb->width + x] = (u8)c | fb->cur_color;
}

finline void plty_fb_lf(plty_fb *fb) {
  fb->cur_x = 0;
  if (fb->cur_y == fb->height - 1) {
    memcpy(fb->vbuf, fb->vbuf + fb->width, 2 * fb->width * (fb->height - 1));
  } else {
    fb->cur_y++;
  }
}

static void plty_fb_putc_raw(plty_fb *fb, int c) {
  u16 ch = (u8)c | fb->cur_color;

  if (c == '\r') {
    fb->cur_x = 0;
    return;
  }

  if (c == '\n') {
    plty_fb_lf(fb);
    return;
  }

  if (fb->cur_x == fb->width) plty_fb_lf(fb); // 当前行已满就换行

  if (c == '\t') {
    size_t next_x = (fb->cur_x & ~7) + 8;
    if (next_x > fb->width) next_x = fb->width;
    for (size_t i = fb->cur_x; i < next_x; i++) {
      fb->vbuf[fb->cur_y * fb->width + i] = ' ' | fb->cur_color;
    }
    fb->cur_x = next_x;
    return;
  }

  fb->vbuf[fb->cur_y * fb->width + fb->cur_x] = ch;
  fb->cur_x++;
}

finline void plty_fb_puts_raw(plty_fb *fb, cstr s) {
  for (; *s != '\0'; s++) {
    plty_fb_putc_raw(fb, *s);
  }
}

finline void plty_fb_applyrgb(plty_fb *fb) {
  int fg = color_best_match(fb->fg);
  int bg = color_best_match(fb->bg);
  plty_fb_setcolor(fb, fg, bg);
}

static void plty_fb_parse(plty_fb *fb) {
  fb->buf[fb->len] = '\0';
  if (fb->len == PLTY_FB_BUFSIZE - 1) goto err;

  char *s = (char *)fb->buf;

  if (*s == '[') {
    if (fb->len == 1) goto again;
    s++;
    char *_s;
    int   n = strb10toi(s, &_s);
    if (s == _s) goto err;
    s = _s;
    if (*s++ != ';') goto err;
    if (n == 38) { // 设置前景
      int n = strb10toi(s, &s);
      if (n == 2) {}
    }
    if (n == 48) { // 设置背景
      int n = strb10toi(s, &s);
    }
  }

  goto end;
err:
  plty_fb_puts_raw(fb, (cstr)fb->buf);
end:
  fb->len = -1; // 退出转义模式
again:
  return;
}

finline void plty_fb_putc(plty_fb *fb, int c) {
  if (fb->len >= 0) { // 处于转义序列中
    fb->buf[fb->len++] = c;
    plty_fb_parse(fb);
    if (fb->len < 0) { // 转义序列结束
      plty_fb_applyrgb(fb);
    }
    return;
  }

  if (c == '\033') { // 转义序列开始
    fb->len = 0;
    return;
  }

  plty_fb_putc_raw(fb, c);
}

finline void plty_fb_puts(plty_fb *fb, cstr s) {
  for (; *s != '\0'; s++) {
    plty_fb_putc(fb, *s);
  }
}

// ----------------------------------------------------------------------------------------------------

static struct font_char empty_char;

plty_t plty_alloc(void *vram, size_t width, size_t height, plff_t font) {
  size_t font_width = (font_getchar(font, 'A') ?: &empty_char)->advance ?: font->height / 2;
  plty_t tty        = malloc(sizeof(*tty));
  if (tty == null) return null;
  tty->vram     = vram;
  tty->width    = width;
  tty->height   = height;
  tty->ncols    = width / font_width;
  tty->nlines   = height / font->height;
  tty->text     = malloc(tty->ncols * tty->nlines * sizeof(*tty->text));
  tty->text2    = malloc(tty->ncols * tty->nlines * sizeof(*tty->text));
  tty->charw    = font_width;
  tty->charh    = font->height + 2;
  tty->fonts[0] = font;
  tty->fonts[1] = null;
  tty->fonts[2] = null;
  tty->fonts[3] = null;
  tty->fg       = (color_t){255, 255, 255, 255};
  tty->bg       = (color_t){0, 0, 0, 255};
  tty->cur_x    = 0;
  tty->cur_y    = 0;
  tty->cur_fg   = tty->fg;
  tty->cur_bg   = tty->bg;
  plty_clear(tty);
  return tty;
}

void plty_free(plty_t tty) {
  free(tty->text);
  free(tty);
}

void plty_addfont(plty_t tty, plff_t font) {
  for (size_t i = 0; i < lengthof(tty->fonts); i++) {
    if (tty->fonts[i] == null) {
      tty->fonts[i] = font;
      return;
    }
  }
}

finline bool cheq(plty_char_t c1, plty_char_t c2) {
  if (c1->ch != c2->ch) return false;
  if (c1->ch) {
    if (c1->fg.r != c2->fg.r) return false;
    if (c1->fg.g != c2->fg.g) return false;
    if (c1->fg.b != c2->fg.b) return false;
  }
  if (c1->bg.r != c2->bg.r) return false;
  if (c1->bg.g != c2->bg.g) return false;
  if (c1->bg.b != c2->bg.b) return false;
  return true;
}

static int vram_putchar(plty_t tty, i32 x, i32 y) {
  struct __PACKED__ {
    byte b, g, r, a;
  }   *vram = tty->vram;
  auto _ch  = &tty->text[y * tty->ncols + x];
  auto _ch2 = &tty->text2[y * tty->ncols + x];
  auto fg   = _ch->fg;
  auto bg   = _ch->bg;
  auto ch   = _ch->ch;
  auto cw   = ch ? (ch->advance + tty->charw - 1) / tty->charw : 1;
  if (cheq(_ch, _ch2)) return cw;
  x *= tty->charw, y *= tty->charh;
  if (ch == null) {
    for (i32 dy = 0; dy < tty->charh; dy++) {
      i32 vy = y + dy;
      if (vy < 0) continue;
      if (vy >= tty->height) break;
      for (i32 dx = 0; dx < tty->charw; dx++) {
        i32 vx = x + dx;
        if (vx < 0) continue;
        if (vx >= tty->width) break;
        vram[vy * tty->width + vx].r = bg.r;
        vram[vy * tty->width + vx].g = bg.g;
        vram[vy * tty->width + vx].b = bg.b;
      }
    }
    return 1;
  }
  auto charw = cw * tty->charw;
  for (i32 dy = 0; dy < ch->top; dy++) {
    i32 vy = y + dy;
    if (vy < 0) continue;
    if (vy >= tty->height) break;
    for (i32 dx = 0; dx < charw; dx++) {
      i32 vx = x + dx;
      if (vx < 0) continue;
      if (vx >= tty->width) break;
      vram[vy * tty->width + vx].r = bg.r;
      vram[vy * tty->width + vx].g = bg.g;
      vram[vy * tty->width + vx].b = bg.b;
    }
  }
  for (i32 dy = ch->top; dy < ch->top + ch->height; dy++) {
    i32 vy = y + dy;
    if (vy < 0) continue;
    if (vy >= tty->height) break;
    for (i32 dx = 0; dx < ch->left; dx++) {
      i32 vx = x + dx;
      if (vx < 0) continue;
      if (vx >= tty->width) break;
      vram[vy * tty->width + vx].r = bg.r;
      vram[vy * tty->width + vx].g = bg.g;
      vram[vy * tty->width + vx].b = bg.b;
    }
    for (i32 dx = ch->left; dx < ch->left + ch->width; dx++) {
      i32 vx = x + dx, cx = dx - ch->left, cy = dy - ch->top;
      if (vx < 0) continue;
      if (vx >= tty->width) break;
      auto kr                      = ch->img[(cy * ch->width + cx) * 3];
      auto kg                      = ch->img[(cy * ch->width + cx) * 3 + 1];
      auto kb                      = ch->img[(cy * ch->width + cx) * 3 + 2];
      vram[vy * tty->width + vx].r = (fg.r * kr + bg.r * (255 - kr)) / 256;
      vram[vy * tty->width + vx].g = (fg.g * kg + bg.g * (255 - kg)) / 256;
      vram[vy * tty->width + vx].b = (fg.b * kb + bg.b * (255 - kb)) / 256;
    }
    for (i32 dx = ch->left + ch->width; dx < charw; dx++) {
      i32 vx = x + dx;
      if (vx < 0) continue;
      if (vx >= tty->width) break;
      vram[vy * tty->width + vx].r = bg.r;
      vram[vy * tty->width + vx].g = bg.g;
      vram[vy * tty->width + vx].b = bg.b;
    }
  }
  for (i32 dy = ch->top + ch->height; dy < tty->charh; dy++) {
    i32 vy = y + dy;
    if (vy < 0) continue;
    if (vy >= tty->height) break;
    for (i32 dx = 0; dx < charw; dx++) {
      i32 vx = x + dx;
      if (vx < 0) continue;
      if (vx >= tty->width) break;
      vram[vy * tty->width + vx].r = bg.r;
      vram[vy * tty->width + vx].g = bg.g;
      vram[vy * tty->width + vx].b = bg.b;
    }
  }
  return cw;
}

void plty_flush(plty_t tty) {
  if (tty == null) return;
  for (i32 y = 0; y < tty->nlines; y++) {
    for (i32 x = 0; x < tty->ncols;) {
      x += vram_putchar(tty, x, y);
    }
  }
  memcpy(tty->text2, tty->text, tty->ncols * tty->nlines * sizeof(*tty->text));
}

void plty_clear(plty_t tty) {
  if (tty == null) return;
  for (u32 i = 0; i < tty->ncols * tty->nlines; i++) {
    tty->text[i].ch = null;
    tty->text[i].fg = tty->fg;
    tty->text[i].bg = tty->bg;
  }
  for (u32 i = 0; i < tty->ncols * tty->nlines; i++) {
    tty->text2[i].ch = &empty_char;
  }
}

void plty_scroll(plty_t tty) {
  if (tty == null) return;
  for (u32 i = 0; i < tty->ncols * (tty->nlines - 1); i++) {
    tty->text[i] = tty->text[i + tty->ncols];
  }
  for (u32 i = tty->ncols * (tty->nlines - 1); i < tty->ncols * tty->nlines; i++) {
    tty->text[i].ch = null;
    tty->text[i].fg = tty->fg;
    tty->text[i].bg = tty->bg;
  }
}

static u32 *utf8to32(cstr s) {
  u32 *r = malloc((strlen(s) + 1) * 4);
  if (r == null) return null;
  size_t i = 0;
  for (; *s != '\0'; i++) {
    if ((*s & 0x80) == 0) {
      r[i] = *s++;
    } else if ((*s & 0xe0) == 0xc0) {
      r[i]  = (*s++ & 0x1f) << 6;
      r[i] |= *s++ & 0x3f;
    } else if ((*s & 0xf0) == 0xe0) {
      r[i]  = (*s++ & 0x0f) << 12;
      r[i] |= (*s++ & 0x3f) << 6;
      r[i] |= *s++ & 0x3f;
    } else if ((*s & 0xf8) == 0xf0) {
      r[i]  = (*s++ & 0x07) << 18;
      r[i] |= (*s++ & 0x3f) << 12;
      r[i] |= (*s++ & 0x3f) << 6;
      r[i] |= *s++ & 0x3f;
    } else {
      r[i] = 0xfffd;
      s++;
    }
  }
  r[i] = '\0';
  return r;
}

color_t plty_getfg(plty_t tty) {
  return tty->cur_fg;
}

color_t plty_getbg(plty_t tty) {
  return tty->cur_bg;
}

void plty_setfg(plty_t tty, color_t fg) {
  tty->cur_fg = fg;
}

void plty_setbg(plty_t tty, color_t bg) {
  tty->cur_bg = bg;
}

color_t plty_getpfg(plty_t tty, i32 x, i32 y) {
  return tty->text[y * tty->ncols + x].fg;
}

color_t plty_getpbg(plty_t tty, i32 x, i32 y) {
  return tty->text[y * tty->ncols + x].bg;
}

void plty_setpfg(plty_t tty, i32 x, i32 y, color_t fg) {
  tty->text[y * tty->ncols + x].fg = fg;
}

void plty_setpbg(plty_t tty, i32 x, i32 y, color_t bg) {
  tty->text[y * tty->ncols + x].bg = bg;
}

u32 plty_getch(plty_t tty, i32 x, i32 y) {
  return tty->text[y * tty->ncols + x].ch->code;
}

static font_char_t plty_getfontch(plty_t tty, u32 ch) {
  font_char_t _ch = null;
#pragma unroll
  for (size_t i = 0; _ch == null && i < lengthof(tty->fonts); i++) {
    _ch = font_getchar(tty->fonts[i], ch);
  }
  return _ch;
}

void plty_setch(plty_t tty, i32 x, i32 y, u32 ch) {
  tty->text[y * tty->ncols + x].ch = plty_getfontch(tty, ch);
  tty->text[y * tty->ncols + x].fg = tty->cur_fg;
  tty->text[y * tty->ncols + x].bg = tty->cur_bg;
}

finline void plty_lf(plty_t tty) {
  for (u32 i = tty->cur_x; i < tty->ncols; i++) {
    tty->text[tty->cur_y * tty->ncols + i].ch = null;
    tty->text[tty->cur_y * tty->ncols + i].fg = tty->cur_fg;
    tty->text[tty->cur_y * tty->ncols + i].bg = tty->cur_bg;
  }
  tty->cur_x = 0;
  if (tty->cur_y == tty->nlines - 1) {
    plty_scroll(tty);
  } else {
    tty->cur_y++;
  }
}

static void plty_putc_raw(plty_t tty, u32 c) {
  if (c == '\r') {
    tty->cur_x = 0;
    return;
  }

  if (c == '\n') {
    plty_lf(tty);
    return;
  }

  auto ch = plty_getfontch(tty, c);
  auto cw = ch ? (ch->advance + tty->charw - 1) / tty->charw : 1;

  if (tty->cur_x + cw > tty->ncols) plty_lf(tty); // 当前行已满就换行

  if (c == '\t') {
    u32 next_x = (tty->cur_x & ~7) + 8;
    if (next_x > tty->ncols) next_x = tty->ncols;
    for (u32 i = tty->cur_x; i < next_x; i++) {
      tty->text[tty->cur_y * tty->ncols + i].ch = null;
      tty->text[tty->cur_y * tty->ncols + i].fg = tty->cur_fg;
      tty->text[tty->cur_y * tty->ncols + i].bg = tty->cur_bg;
    }
    tty->cur_x = next_x;
    return;
  }

  if (c == ' ') {
    tty->text[tty->cur_y * tty->ncols + tty->cur_x].ch = null;
    tty->text[tty->cur_y * tty->ncols + tty->cur_x].fg = tty->cur_fg;
    tty->text[tty->cur_y * tty->ncols + tty->cur_x].bg = tty->cur_bg;
  } else {
    plty_setch(tty, tty->cur_x, tty->cur_y, c);
  }

  for (u32 i = 1; i < cw; i++) {
    tty->text[tty->cur_y * tty->ncols + tty->cur_x + 1].ch = &empty_char;
  }
  tty->cur_x += cw;
}

static void plty_puts_raw(plty_t tty, const u32 *s) {
  for (; *s != '\0'; s++) {
    plty_putc_raw(tty, *s);
  }
}

void plty_putc(plty_t tty, int c) {
  plty_putc_raw(tty, c);
}

void plty_puts(plty_t tty, cstr s) {
  auto utf32s = utf8to32(s);
  if (utf32s == null) return;
  plty_puts_raw(tty, utf32s);
  free(utf32s);
}
