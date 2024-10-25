#include <kernel.h>
#include <plty.h>

void plty_tty_putchar(struct tty *tty, int ch) {
  plty_t plty = (plty_t)tty->vram;
  plty_setbg(plty, color_table_16[tty->color >> 8]);
  plty_setfg(plty, color_table_16[tty->color & 0xf]);
  if (ch == '\b') {
    tty->gotoxy(tty, tty->x - 1, tty->y);
    plty_putc(plty, ' ');
    tty->x = plty_getcurx(plty);
    tty->y = plty_getcury(plty);
    tty->gotoxy(tty, tty->x - 1, tty->y);
    plty_flush(plty);
  } else {
    plty_pututf8c(plty, ch);
    if (ch == '\n' || ch == '\r') plty_flush(plty);
    tty->x = plty_getcurx(plty);
    tty->y = plty_getcury(plty);
  }
}

void plty_tty_move_cursor(struct tty *tty, int x, int y) {
  plty_t plty = (plty_t)tty->vram;
  plty_setcur(plty, x, y);
  plty_flush(plty);
  tty->x = plty_getcurx(plty);
  tty->y = plty_getcury(plty);
}

void plty_tty_screen_ne(struct tty *tty) {
  plty_t plty = (plty_t)tty->vram;
  plty_scroll(plty);
}

void plty_tty_clear(struct tty *tty) {
  plty_t plty = (plty_t)tty->vram;
  plty_clear(plty);
}

static int default_tty_fifo_status(struct tty *res) {
  return cir_queue8_len(task_get_key_queue(current_task()));
}

static int default_tty_fifo_get(struct tty *res) {
  return cir_queue8_get(task_get_key_queue(current_task()));
}
void plty_tty_flush(struct tty *tty) {
  plty_t plty = (plty_t)tty->vram;
  plty_flush(plty);
}
struct tty *plty_set_tty(plty_t plty) {
  plty->auto_flush = false;
  plty->show_cur   = true;
  struct tty *tty  = tty_alloc(plty, plty->ncols, plty->nlines, plty_tty_putchar,
                               plty_tty_move_cursor, plty_tty_clear, plty_tty_screen_ne,
                               default_tty_fifo_status, default_tty_fifo_get, plty_tty_flush);
  return tty;
}

extern struct tty *tty_default;

void plty_set_default(plty_t plty) {
  tty_default         = plty_set_tty(plty);
  current_task()->TTY = tty_default;
}
