#include "base/data-structure/circular-queue.h"
#include <kernel.h>
list_t      tty_list = null;
struct tty *tty_default;

void t_putchar(struct tty *res, char ch);

static void tty_print(struct tty *res, cstr string) {
  size_t len = strlen(string);
  for (int i = 0; i < len; i++) {
    if (res->y == res->ysize && res->x >= res->xsize) { res->screen_ne(res); }
    t_putchar(res, string[i]);
  }
}

static void tty_gotoxy(struct tty *res, int x, int y) {
  if (res->x == x && res->y == y) return;
  if (x >= 0 && y >= 0) {
    int x2 = x;
    int y2 = y;
    if (x <= res->xsize - 1 && y <= res->ysize - 1) {
      res->MoveCursor(res, x, y);
      return;
    }
    if (x <= res->xsize - 1) {
      for (int i = 0; i < y - res->ysize + 1; i++) {
        res->screen_ne(res);
      }
      res->MoveCursor(res, x, res->ysize - 1);
      return;
    }
    if (x > res->xsize - 1) {
      y2 += x / res->xsize - 1;
      x2  = x % res->xsize;
      if (y2 <= res->ysize - 1)
        tty_gotoxy(res, x2, y2 + 1);
      else
        tty_gotoxy(res, x2, y2);
    }
  } else {
    if (x < 0) {
      x += res->xsize;
      y--;
      tty_gotoxy(res, x, y);
    }
    if (y < 0) { return; }
  }
}

int default_tty_fifo_status(struct tty *res) {
  return cir_queue_len(task_get_key_queue(current_task()));
}
int default_tty_fifo_get(struct tty *res) {
  return cir_queue_get(task_get_key_queue(current_task()));
}

void init_tty() {
  tty_default = tty_alloc((void *)0xb8000, 80, 25, putchar_TextMode, MoveCursor_TextMode,
                          clear_TextMode, screen_ne_TextMode, Draw_Box_TextMode,
                          default_tty_fifo_status, default_tty_fifo_get);
}

struct tty *tty_alloc(void *vram, int xsize, int ysize, void (*putchar)(struct tty *res, int c),
                      void (*MoveCursor)(struct tty *res, int x, int y),
                      void (*clear)(struct tty *res), void (*screen_ne)(struct tty *res),
                      void (*Draw_Box)(struct tty *res, int x, int y, int x1, int y1, u8 color),
                      int (*fifo_status)(struct tty *res), int (*fifo_get)(struct tty *res)) {
  struct tty *res  = (struct tty *)page_malloc(sizeof(struct tty));
  res->is_using    = 1;
  res->x           = 0;
  res->y           = 0;
  res->vram        = vram;
  res->xsize       = xsize;
  res->ysize       = ysize;
  res->putchar     = putchar;
  res->MoveCursor  = MoveCursor;
  res->clear       = clear;
  res->screen_ne   = screen_ne;
  res->Draw_Box    = Draw_Box;
  res->gotoxy      = tty_gotoxy;
  res->print       = tty_print;
  res->fifo_status = fifo_status;
  res->fifo_get    = fifo_get;
  res->color       = 0x07;
  res->cur_moving  = 1;
  res->vt100       = 0;
  res->buf_p       = 0;
  memset(res->buffer, 0, sizeof(res->buffer));
  res->done        = 0;
  res->mode        = 0;
  res->color_saved = -1;
  list_append(tty_list, res);
  return res;
}
void tty_free(struct tty *res) {
  list_delete(tty_list, res);
  page_free((void *)res, sizeof(struct tty));
}
struct tty *tty_set(mtask *task, struct tty *res) {
  if (res->is_using == 1) {
    struct tty *old = task->TTY;
    task->TTY       = res;
    return old;
  }
  return NULL;
}
struct tty *tty_set_default(struct tty *res) {
  if (res->is_using == 1) {
    struct tty *old = tty_default;
    tty_default     = res;
    return old;
  }
  return NULL;
}
void tty_set_reserved(struct tty *res, u32 reserved1, u32 reserved2, u32 reserved3, u32 reserved4) {
  res->reserved[0] = reserved1;
  res->reserved[1] = reserved2;
  res->reserved[2] = reserved3;
  res->reserved[3] = reserved4;
}
void tty_stop_cursor_moving(struct tty *t) {
  t->cur_moving = 0;
}
void tty_start_curor_moving(struct tty *t) {
  t->cur_moving = 1;
  t->MoveCursor(t, t->x, t->y);
}