#include <kernel.h>

static int get_cursor_idx() {
  asm_out8(0x03d4, 0x0e);
  byte hi = asm_in8(0x03d5);
  asm_out8(0x03d4, 0x0f);
  byte lo = asm_in8(0x03d5);
  return (hi << 8) + lo;
}

static void set_cursor_idx(int idx) {
  asm_out8(0x03d4, 0x0e);
  asm_out8(0x03d5, idx >> 8);
  asm_out8(0x03d4, 0x0f);
  asm_out8(0x03d5, idx & 0xff);
}

void MoveCursor_TextMode(struct tty *res, int x, int y) {
  res->x = x;
  res->y = y;
  if (!res->cur_moving) return;
  int i = y * res->xsize + x;
  if (res->vram == (void *)0xb8000) { // TODO 移除特判
    set_cursor_idx(i);
  }
}

void putchar_TextMode(struct tty *res, int c) {
  if (res->x == res->xsize) { res->gotoxy(res, 0, res->y + 1); }
  if (c == '\n') {
    if (res->y == res->ysize - 1) {
      res->screen_ne(res);
      return;
    }
    res->MoveCursor(res, 0, res->y + 1);
    return;
  } else if (c == '\0') {
    return;
  } else if (c == '\b') {
    if (res->x == 0) {
      res->MoveCursor(res, res->xsize - 1, res->y - 1);
      *(byte *)(res->vram + res->y * res->xsize * 2 + res->x * 2)         = ' ';
      *(byte *)(res->vram + res->y * res->xsize * 2 + res->x * 2 - 2 + 1) = res->color;
      return;
    }
    *(byte *)(res->vram + res->y * res->xsize * 2 + res->x * 2 - 2)     = ' ';
    *(byte *)(res->vram + res->y * res->xsize * 2 + res->x * 2 - 2 + 1) = res->color;
    res->MoveCursor(res, res->x - 1, res->y);
    return;
  } else if (c == '\t') { // 制表符
    res->print(res, "    ");
    return;
  } else if (c == '\r') {
    return;
  }
  *(byte *)(res->vram + res->y * res->xsize * 2 + res->x * 2)     = c;
  *(byte *)(res->vram + res->y * res->xsize * 2 + res->x * 2 + 1) = res->color;
  res->MoveCursor(res, res->x + 1, res->y);
}

void screen_ne_TextMode(struct tty *res) {
  for (int i = 0; i < res->xsize * 2; i += 2) {
    for (int j = 0; j < res->ysize; j++) {
      *(byte *)(res->vram + j * res->xsize * 2 + i) =
          *(byte *)(res->vram + (j + 1) * res->xsize * 2 + i);
      *(byte *)(res->vram + j * res->xsize * 2 + i + 1) =
          *(byte *)(res->vram + (j + 1) * res->xsize * 2 + i + 1);
    }
  }
  for (int i = 0; i < res->xsize * 2; i += 2) {
    *(byte *)(res->vram + (res->ysize - 1) * res->xsize * 2 + i)     = ' ';
    *(byte *)(res->vram + (res->ysize - 1) * res->xsize * 2 + i + 1) = res->color;
  }
  res->gotoxy(res, 0, res->ysize - 1);
  res->Raw_y++;
}

void clear_TextMode(struct tty *res) {
  for (int i = 0; i < res->xsize * 2; i += 2) {
    for (int j = 0; j < res->ysize; j++) {
      *(byte *)(res->vram + j * res->xsize * 2 + i)     = ' ';
      *(byte *)(res->vram + j * res->xsize * 2 + i + 1) = res->color;
    }
  }
  res->gotoxy(res, 0, 0);
  res->Raw_y = 0;
}
