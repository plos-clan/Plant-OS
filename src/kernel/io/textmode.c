#include <kernel.h>

void MoveCursor_TextMode(struct tty *res, int x, int y) {
  res->x = x;
  res->y = y;
  if (!res->cur_moving) return;
  int i = y * res->xsize + x;
  if (res->vram == 0xb8000) { move_cursor_by_idx(i); }
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
      *(unsigned char *)(res->vram + res->y * res->xsize * 2 + res->x * 2)         = ' ';
      *(unsigned char *)(res->vram + res->y * res->xsize * 2 + res->x * 2 - 2 + 1) = res->color;
      return;
    }
    *(unsigned char *)(res->vram + res->y * res->xsize * 2 + res->x * 2 - 2)     = ' ';
    *(unsigned char *)(res->vram + res->y * res->xsize * 2 + res->x * 2 - 2 + 1) = res->color;
    res->MoveCursor(res, res->x - 1, res->y);
    return;
  } else if (c == '\t') {
    // 制表符
    res->print(res, "    ");
    return;
  } else if (c == '\r') {
    return;
  }
  *(unsigned char *)(res->vram + res->y * res->xsize * 2 + res->x * 2)     = c;
  *(unsigned char *)(res->vram + res->y * res->xsize * 2 + res->x * 2 + 1) = res->color;
  res->MoveCursor(res, res->x + 1, res->y);
}

void screen_ne_TextMode(struct tty *res) {
  for (int i = 0; i < res->xsize * 2; i += 2) {
    for (int j = 0; j < res->ysize; j++) {
      *(unsigned char *)(res->vram + j * res->xsize * 2 + i) =
          *(unsigned char *)(res->vram + (j + 1) * res->xsize * 2 + i);
      *(unsigned char *)(res->vram + j * res->xsize * 2 + i + 1) =
          *(unsigned char *)(res->vram + (j + 1) * res->xsize * 2 + i + 1);
    }
  }
  for (int i = 0; i < res->xsize * 2; i += 2) {
    *(unsigned char *)(res->vram + (res->ysize - 1) * res->xsize * 2 + i)     = ' ';
    *(unsigned char *)(res->vram + (res->ysize - 1) * res->xsize * 2 + i + 1) = res->color;
  }
  res->gotoxy(res, 0, res->ysize - 1);
  res->Raw_y++;
}

void clear_TextMode(struct tty *res) {
  for (int i = 0; i < res->xsize * 2; i += 2) {
    for (int j = 0; j < res->ysize; j++) {
      *(unsigned char *)(res->vram + j * res->xsize * 2 + i)     = ' ';
      *(unsigned char *)(res->vram + j * res->xsize * 2 + i + 1) = res->color;
    }
  }
  res->gotoxy(res, 0, 0);
  res->Raw_y = 0;
}
void Draw_Box_TextMode(struct tty *res, int x, int y, int x1, int y1, unsigned char color) {
  for (int i = y; i < y1; i++) {
    for (int j = x; j < x1; j++) {
      *(unsigned char *)(res->vram + i * 160 + j * 2 + 1) = color;
    }
  }
}

void AddShell_TextMode() {
  // char *vram = page_malloc(160 * 25);
  // struct tty *ntty =
  //     tty_alloc(vram, 80, 25, putchar_TextMode, MoveCursor_TextMode,
  //               clear_TextMode, screen_ne_TextMode, Draw_Box_TextMode);
  // io_cli();
  // mtask *ntask =
  //     register_task("Shell", 1, 2 * 8, (int)shell_handler, 1 * 8, 1 * 8,
  //             (unsigned int)page_malloc(128 * 1024) + 128 * 1024);
  // char *kfifo = (struct FIFO8 *)page_malloc(sizeof(struct FIFO8));
  // char *mfifo = (struct FIFO8 *)page_malloc(sizeof(struct FIFO8));
  // char *kbuf = (char *)page_malloc(4096);
  // char *mbuf = (char *)page_malloc(4096);
  // fifo8_init(kfifo, 4096, kbuf);
  // fifo8_init(mfifo, 4096, mbuf);
  // task_set_fifo(ntask, kfifo, mfifo);
  // void *alloc_addr = (void *)page_malloc(512 * 1024);
  // ntask->alloc_addr = alloc_addr;
  // ntask->alloc_size = 512 * 1024;
  // ntask->mm = memory_init((uint32_t)alloc_addr, 512 * 1024);
  // ntask->fifosleep = 3;
  // int fg = tty_set(ntask, ntty);
  // //  printk("set vram = %08x\n",ntty->vram);
  // ntty->clear(ntty);
  // io_sti();
}

void SwitchShell_TextMode(int i) {
  // io_cli();
  // extern struct List *tty_list;
  // extern struct tty *tty_default;
  // struct tty *t = (struct tty *)FindForCount(i, tty_list)->val;
  // struct tty *n = NULL;
  // for (int j = 1; FindForCount(j, tty_list) != 0; j++) {
  //   n = (struct tty *)FindForCount(j, tty_list)->val;
  //   if (n->vram == 0xb8000) {
  //     break;
  //   } else {
  //     n = NULL;
  //   }
  // }
  // if (n == NULL) {
  //   n = tty_default;
  // }
  // if (n == t) {
  //   return;
  // }
  // // 交换
  // unsigned char *buf = page_malloc(160 * 25);
  // memcpy(buf, t->vram, 160 * 25);
  // memcpy(t->vram, n->vram, 160 * 25);
  // memcpy(n->vram, buf, 160 * 25);
  // for (int j = 1; get_task(j) != 0; j++) {
  //   mtask *task = get_task(j);
  //   if (task->TTY == t && (strcmp("Shell", task->name) == 0 ||
  //                          (task->app == 1 && task->forever == 0))) {
  //     task->sleep = 0;
  //     if (task->fifosleep == 3) {
  //       task->fifosleep = 0;
  //     }
  //   } else if ((task->TTY == n || task->TTY->is_using != 1) &&
  //              (strcmp("Shell", task->name) == 0 ||
  //               (task->app == 1 && task->forever == 0))) {
  //     if (task->fifosleep == 0) {
  //       task->fifosleep = 3;
  //     }
  //   }
  // }
  // page_free(buf, 160 * 25);
  // buf = t->vram;
  // t->vram = n->vram;
  // n->vram = buf;
  // t->MoveCursor(t, t->x, t->y);
  // io_sti();
}

bool now_tty_TextMode(struct tty *res) {
  if (res->vram == 0xb8000) {
    return true;
  } else {
    return false;
  }
}