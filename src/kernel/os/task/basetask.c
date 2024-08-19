// This code is released under the MIT License

#include <font.h>
#include <fs.h>
#include <kernel.h>
#include <pl_readline.h>
#include <plty.h>

u8  *shell_data;
void ide_initialize(u32 BAR0, u32 BAR1, u32 BAR2, u32 BAR3, u32 BAR4);
void sound_test();
int  os_execute(char *filename, char *line);

void idle_loop() {
  infinite_loop task_next();
}
void insert_char(char *str, int pos, char ch);
void delete_char(char *str, int pos);

#if 0
ssize_t input(char *ptr, size_t len) {
  size_t p = 0;
  ptr[p]   = '\0';

  int c;
  while (p < len - 1 && (c = getch()) != '\n') {
    if (c == '\b') {
      if (p == 0) continue;
      p--;
      ptr[p] = '\0';
      int x = get_x() - 1, y = get_y();
      gotoxy(x, y);
      putchar(' ');
      gotoxy(x, y);
      continue;
    }

    if (c > 0) {
      ptr[p++] = c;
      ptr[p]   = '\0';
      putchar(c);
      continue;
    }

    if (c == -1) {
      // up
    } else if (c == -2) {
      // down
    } else if (c == -3) {
      // left
    } else if (c == -4) {
      // right
    }
  }

  putchar('\n');

  return p;
}
#endif

void list_files(char *path) {
  klogd("%s", path);
  vfs_node_t p = vfs_open(path);
  assert(p, "open %s failed", path);
  assert(p->type == file_dir);
  list_foreach(p->child, i) {
    vfs_node_t c = (vfs_node_t)i->data;
    printf("%!8s ", c->name);
  }
  printf("\n");
}

int readline_getch() {
  int ch;
  while ((ch = getch()) == 0) {}
  if (ch == -1) return PL_READLINE_KEY_UP;
  if (ch == -2) return PL_READLINE_KEY_DOWN;
  if (ch == -3) return PL_READLINE_KEY_LEFT;
  if (ch == -4) return PL_READLINE_KEY_RIGHT;
  if (ch == '\n') return PL_READLINE_KEY_ENTER;
  if (ch == '\b') return PL_READLINE_KEY_BACKSPACE;
  if (ch == '\t') return PL_READLINE_KEY_TAB;
  return ch;
}

void shell() {
  printi("shell has been started");
  void *kfifo = page_malloc_one();
  void *kbuf  = page_malloc_one();
  cir_queue8_init(kfifo, 0x1000, kbuf);
  current_task()->keyfifo = (cir_queue8_t)kfifo;
  char         *path      = malloc(1024);
  char         *ch        = malloc(255);
  pl_readline_t n;
  n = pl_readline_init(readline_getch, putchar);
  sprintf(path, "/");
  while (true) {
    printf("%s# ", path);
    pl_readline(n, ch, 255);

    if (strneq(ch, "cd ", 3)) {
      char *s = ch + 3;
      if (streq(s, ".")) continue;
      if (streq(s, "..")) {
        if (streq(s, "/")) continue;
        char *n = path + strlen(path);
        while (*--n != '/' && n != path) {}
        *n = '\0';
        if (strlen(path) == 0) strcpy(path, "/");
        continue;
      }
      if (streq(path, "/"))
        sprintf(path, "%s%s", path, s);
      else
        sprintf(path, "%s/%s", path, s);
    } else if (strneq(ch, "exec ", 5)) {
      char *s  = ch + 5;
      char *s2 = strchr(s, ' ');
      if (s2) *s2 = '\0';
      char *s3   = strdup(s);
      *s2        = ' ';
      int status = os_execute(s3, s);
      printf("%s exited with code %d\n", s3, status);
      free(s3);
    } else if (streq(ch, "ls")) {
      list_files(path);
    } else {
      printf("bad command\n");
    }
  }
}

void scan_files(char *path) {
  vfs_node_t p = vfs_open(path);
  assert(p, "open %s failed", path);
  assert(p->type == file_dir);
  list_foreach(p->child, i) {
    vfs_node_t c        = (vfs_node_t)i->data;
    char      *new_path = pathcat(path, c->name);
    if (c->type == file_dir) {
      scan_files(new_path);
    } else {
      printf("%s\n", new_path);
    }
    free(new_path);
  }
}

plff_t load_font(cstr path) {
  auto  file = vfs_open(path);
  byte *buf  = malloc(file->size);
  vfs_read(file, buf, 0, file->size);
  auto font = plff_load_from_mem(buf, file->size);
  klogi("font %s: %d %d %d", path, font->nchars, font->height, font->channels);
  free(buf);
  return font;
}

void plty_set_default(plty_t plty);

void init() {
  klogd("init function has been called successfully!");
  printf("Hello Plant-OS!\n");

  // klogd("Set Mode");
  byte *vram = (void *)set_mode(1024, 768, 32);
  klogd("ok vram = %p", vram);
  memset(vram, 0, 1024 * 768 * 4);
  floppy_init();
  ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);

  int s = 0x41;
  vfs_mkdir("/fatfs0");
  vfs_mount((cstr)&s, vfs_open("/fatfs0"));
  s++;
  vfs_mkdir("/fatfs1");
  vfs_mount((cstr)&s, vfs_open("/fatfs1"));

  auto font1 = load_font("/fatfs1/font1.plff");
  // // auto font2 = load_font("/fatfs1/font2.plff");

  auto tty = plty_alloc(vram, 1024, 768, font1);
  // plty_addfont(tty, font2);

  plty_set_default(tty);

  // info("Plant-OS 终端现在支持中文啦！");

  // for (int i = 0;; i++) {
  //   info("Hello world! %d", i);
  // }

  // const int bx = 200, by = 200, r = 100;
  // for (int y = 0; y < 1024; y++) {
  //   for (int x = 0; x < 768; x++) {
  //     struct __PACKED__ {
  //       byte b, g, r, a;
  //     } *const buf = (void *)vram;
  //     if ((x - bx) * (x - bx) + (y - by) * (y - by) >= (r - 1) * (r - 1) &&
  //         (x - bx) * (x - bx) + (y - by) * (y - by) < r * r) {
  //       buf[y * 1024 + x].r = 255;
  //       buf[y * 1024 + x].g = 255;
  //       buf[y * 1024 + x].b = 255;
  //     } else {
  //       buf[y * 1024 + x].r = 0;
  //       buf[y * 1024 + x].g = 0;
  //       buf[y * 1024 + x].b = 0;
  //     }
  //   }
  // }

  // infinite_loop;

  // for (int i = 0;; i++) {
  //   static char text[1024];
  //   sprintf(text, "Hello world!  %d\n", i + 1);
  //   color_t color = {rand(), rand(), rand(), 255};
  //   plty_setfg(tty, color);
  //   color_t color2 = {rand(), rand(), rand(), 255};
  //   plty_setbg(tty, color2);
  //   plty_puts(tty, text);
  //   plty_flush(tty);
  // }

  create_task((u32)shell, 0, 1, 1);
  create_task((u32)sound_test, 0, 1, 1);

  printi("%d alloced pages", page_get_alloced());
  int status = os_execute("/fatfs1/testapp-cpp.bin", "/fatfs1/testapp-cpp.bin 1 2 3");
  printi("TESTAPP-CPP.BIN exit with code %d", status);
  printi("%d alloced pages", page_get_alloced());

  infinite_loop task_next();
}
