// This code is released under the MIT License

#include "color-match.h"
#include <plty.h>

// TODO 完成优化

// ----------------------------------------------------------------------------------------------------
//; 字体缓存

static void fontbuf_free(plty_fontbuf_t buf) {
  if (buf == null) return;
  for (size_t i = 0; i < 256; i++) {
    if (buf[i] == null) continue;
    for (size_t j = 0; j < 256; j++) {
      if (buf[i][j] == null) continue;
      free(buf[i][j]);
    }
    free(buf[i]);
  }
  free(buf);
}

static void *getfont(plty_t tty, u32 ch) {
  return null; // TODO
}

void plty_clear_fontbuf(plty_t tty) {
  if (tty == null) return;
  fontbuf_free(tty->fontbuf);
  tty->fontbuf = null;
}
// ----------------------------------------------------------------------------------------------------

static struct font_char empty_char;

plty_t plty_alloc(void *vram, size_t width, size_t height, plff_t font) {
  if (vram == null || width == 0 || height == 0 || font == null) return null;
  if (width > 131072 || height > 131072) ERROR_P(1, "tty too large");
  size_t font_width  = ((font_getchar(font, 'A') ?: &empty_char)->advance ?: font->height / 2) + 1;
  size_t font_height = font->height + 2;
  if (font_width > 32768 || font_height > 32768) ERROR_P(1, "font too large");
  plty_t tty = malloc(sizeof(*tty));
  if (tty == null) ERROR_P(1, "out of memory");
  tty->vram       = vram;
  tty->backbuf    = null;
  tty->width      = width;
  tty->height     = height;
  tty->ncols      = width / font_width;
  tty->nlines     = height / font_height;
  tty->text       = malloc(tty->ncols * tty->nlines * sizeof(*tty->text));
  tty->text2      = malloc(tty->ncols * tty->nlines * sizeof(*tty->text));
  tty->charw      = font_width;
  tty->charh      = font_height;
  tty->fonts[0]   = font;
  tty->fonts[1]   = null;
  tty->fonts[2]   = null;
  tty->fonts[3]   = null;
  tty->fontbuf    = null;
  tty->fg         = (color_t){255, 255, 255, 255};
  tty->bg         = (color_t){0, 0, 0, 255};
  tty->cur_x      = 0;
  tty->cur_y      = 0;
  tty->cur_fg     = tty->fg;
  tty->cur_bg     = tty->bg;
  tty->auto_flush = false;
  tty->show_cur   = false;
  tty->cur_style  = plty_cur_block;
  tty->flipbuf    = null;
  tty->dirty      = false;
  if (tty->text == null || tty->text2 == null) {
    free(tty->text);
    free(tty->text2);
    free(tty);
    ERROR_P(1, "out of memory");
  }
  plty_clear(tty);
  return tty;
}

void plty_free(plty_t tty) {
  free(tty->text);
  free(tty);
}

void plty_setbackbuffer(plty_t tty, void *vram, void (*flipbuf)()) {
  if (tty == null || (vram == null) != (flipbuf == null)) return;
  for (u32 i = 0; i < tty->ncols * tty->nlines; i++) {
    tty->text2[i].ch = &empty_char;
  }
  tty->backbuf = vram;
  tty->flipbuf = flipbuf;
}

void plty_addfont(plty_t tty, plff_t font) {
#pragma unroll
  for (size_t i = 0; i < lengthof(tty->fonts); i++) {
    if (tty->fonts[i] == null) {
      tty->fonts[i] = font;
      return;
    }
  }
}

// 判断两个字符缓存是否相同
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

// 在显存中输出一个字符
static int vram_putchar(plty_t tty, i32 x, i32 y, bool force) {
  struct __PACKED__ {
    byte b, g, r, a;
  } vbuf, *const vram = tty->vram;
  val _ch  = &tty->text[y * tty->ncols + x];
  val _ch2 = &tty->text2[y * tty->ncols + x];
  var fg   = _ch->fg;
  var bg   = _ch->bg;
  val ch   = _ch->ch;
  val cw   = ch ? (ch->advance + tty->charw - 1) / tty->charw : 1;
  if (tty->backbuf == null && !force && cheq(_ch, _ch2)) return cw;
  if (x == tty->cur_x && y == tty->cur_y && _ch->ch == null) {
    fg = tty->cur_fg;
    bg = tty->cur_bg;
  }
  if (force && x == tty->cur_x && y == tty->cur_y) {
    auto tmp = fg;
    fg       = bg;
    bg       = tmp;
  }
  x *= tty->charw, y *= tty->charh;
  val charw = cw * tty->charw;
  for (i32 dy = 0; dy < tty->charh; dy++) {
    const i32  vy = y + dy;
    const bool by = dy < ch->top || dy >= ch->top + ch->height;
    for (i32 dx = 0; dx < charw; dx++) {
      const i32  vx    = x + dx;
      const bool bx    = dx < ch->left || dx >= ch->left + ch->width;
      val        vramp = &vram[vy * tty->width + vx];
      if (ch == null || by || bx) {
        vbuf.r = bg.r;
        vbuf.g = bg.g;
        vbuf.b = bg.b;
      } else {
        const i32 cx = dx - ch->left, cy = dy - ch->top;
        auto      kr = ch->img[(cy * ch->width + cx) * 3];
        auto      kg = ch->img[(cy * ch->width + cx) * 3 + 1];
        auto      kb = ch->img[(cy * ch->width + cx) * 3 + 2];
        vbuf.r       = (fg.r * kr + bg.r * (255 - kr)) / 256;
        vbuf.g       = (fg.g * kg + bg.g * (255 - kg)) / 256;
        vbuf.b       = (fg.b * kb + bg.b * (255 - kb)) / 256;
      }
      vbuf.a = 255;
      *vramp = vbuf;
    }
  }
  return cw;
}

// 刷新显存
static void plty_do_flush(plty_t tty) {
  if (tty->backbuf != null) {
    void *temp   = tty->vram;
    tty->vram    = tty->backbuf;
    tty->backbuf = temp;
  }
  for (i32 y = 0; y < tty->nlines; y++) {
    for (i32 x = 0; x < tty->ncols;) {
      x += vram_putchar(tty, x, y, false);
    }
  }
  if (tty->show_cur) {
    vram_putchar(tty, tty->cur_oldx, tty->cur_oldy, true);
    vram_putchar(tty, tty->cur_x, tty->cur_y, true);
  }
  tty->cur_oldx = tty->cur_x;
  tty->cur_oldy = tty->cur_y;
  if (tty->backbuf != null) {
    tty->flipbuf();
    return;
  }
  memcpy(tty->text2, tty->text, tty->ncols * tty->nlines * sizeof(*tty->text));
}

void plty_flush(plty_t tty) {
  if (tty == null || !tty->dirty) return;
  plty_do_flush(tty);
}

void plty_clear(plty_t tty) {
  if (tty == null) return;
  tty->dirty = true;
  for (u32 i = 0; i < tty->ncols * tty->nlines; i++) {
    tty->text[i].ch = null;
    tty->text[i].fg = tty->fg;
    tty->text[i].bg = tty->bg;
  }
  if (tty->backbuf != null) return;
  for (u32 i = 0; i < tty->ncols * tty->nlines; i++) {
    tty->text2[i].ch = &empty_char;
  }
}

void plty_scroll(plty_t tty) {
  if (tty == null) return;
  tty->dirty = true;
  for (u32 i = 0; i < tty->ncols * (tty->nlines - 1); i++) {
    tty->text[i] = tty->text[i + tty->ncols];
  }
  for (u32 i = tty->ncols * (tty->nlines - 1); i < tty->ncols * tty->nlines; i++) {
    tty->text[i].ch = null;
    tty->text[i].fg = tty->fg;
    tty->text[i].bg = tty->bg;
  }
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
  tty->dirty                       = true;
  tty->text[y * tty->ncols + x].fg = fg;
}

void plty_setpbg(plty_t tty, i32 x, i32 y, color_t bg) {
  tty->dirty                       = true;
  tty->text[y * tty->ncols + x].bg = bg;
}

u32 plty_getch(plty_t tty, i32 x, i32 y) {
  return tty->text[y * tty->ncols + x].ch->code;
}

// 获取字符的字形
static font_char_t plty_getfontch(plty_t tty, u32 ch) {
  font_char_t _ch = null;
#pragma unroll
  for (size_t i = 0; i < lengthof(tty->fonts); i++) {
    _ch = _ch ?: font_getchar(tty->fonts[i], ch);
  }
  return _ch;
}

void plty_setch(plty_t tty, i32 x, i32 y, u32 ch) {
  tty->dirty                       = true;
  tty->text[y * tty->ncols + x].ch = plty_getfontch(tty, ch);
  tty->text[y * tty->ncols + x].fg = tty->cur_fg;
  tty->text[y * tty->ncols + x].bg = tty->cur_bg;
}

void plty_getcur(plty_t tty, u32 *x, u32 *y) {
  *x = tty->cur_x;
  *y = tty->cur_y;
}

u32 plty_getcurx(plty_t tty) {
  return tty->cur_x;
}

u32 plty_getcury(plty_t tty) {
  return tty->cur_y;
}

void plty_setcur(plty_t tty, u32 x, u32 y) {
  if (tty->show_cur) tty->dirty = true;
  size_t i = y * tty->ncols + x;
  if (i >= tty->ncols * tty->nlines) i = tty->ncols * tty->nlines - 1;
  while (tty->text[i].ch == &empty_char) {
    i--;
  }
  tty->cur_x = i % tty->ncols;
  tty->cur_y = i / tty->ncols;
}

void plty_curnext(plty_t tty, u32 n) {
  if (tty->show_cur) tty->dirty = true;
  size_t i = tty->cur_y * tty->ncols + tty->cur_x;
  for (; n > 0; i++) {
    if (i == tty->ncols * tty->nlines - 1) {
      while (tty->text[i].ch == &empty_char)
        i--;
      break;
    }
    if (tty->text[i].ch != &empty_char) n--;
  }
  tty->cur_x = i % tty->ncols;
  tty->cur_y = i / tty->ncols;
}

void plty_curprev(plty_t tty, u32 n) {
  if (tty->show_cur) tty->dirty = true;
  size_t i = tty->cur_y * tty->ncols + tty->cur_x;
  for (; n > 0; i--) {
    if (i == 0) break;
    if (tty->text[i].ch != &empty_char) n--;
  }
  tty->cur_x = i % tty->ncols;
  tty->cur_y = i / tty->ncols;
}

// 换行
static void plty_lf(plty_t tty) {
  tty->cur_x = 0;
  if (tty->cur_y == tty->nlines - 1) {
    plty_scroll(tty);
  } else {
    tty->cur_y++;
  }
  if (tty->auto_flush) plty_flush(tty);
}

static void plty_putc_raw(plty_t tty, u32 c) {
  if (tty->show_cur) tty->dirty = true;
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

  for (size_t i = tty->cur_x; i < tty->ncols; i++) {
    auto ptr = &tty->text[tty->cur_y * tty->ncols + i];
    if (ptr->ch != &empty_char) break;
    ptr->ch = null;
    ptr->fg = tty->fg;
    ptr->bg = tty->bg;
  }
}

static void plty_puts_raw(plty_t tty, const u32 *s) {
  for (; *s != '\0'; s++) {
    plty_putc_raw(tty, *s);
  }
}

void plty_putc(plty_t tty, u32 c) {
  plty_putc_raw(tty, c);
}

void plty_pututf8c(plty_t tty, byte c) {
  static u32 ch;
  static i32 nneed = -1;
  if (nneed < 0) {
    if ((c & 0x80) == 0) {
      ch    = c;
      nneed = 0;
    } else if ((c & 0xe0) == 0xc0) {
      ch    = (c & 0x1f) << 6;
      nneed = 1;
    } else if ((c & 0xf0) == 0xe0) {
      ch    = (c & 0x0f) << 12;
      nneed = 2;
    } else if ((c & 0xf8) == 0xf0) {
      ch    = (c & 0x07) << 18;
      nneed = 3;
    } else {
      ch    = 0xfffd;
      nneed = 0;
    }
  } else if ((c & 0xc0) == 0x80) {
    nneed--;
    ch |= (c & 0x3f) << (nneed * 6);
  } else {
    ch    = 0xfffd;
    nneed = 0;
  }
  if (nneed == 0) {
    plty_putc(tty, ch);
    nneed = -1;
  }
}

void plty_puts(plty_t tty, cstr _s) {
  cstr s = _s;
  for (; *s != '\0';) {
    plty_putc_raw(tty, utf8to32c((cstr8 *)&s));
  }
}
