#include "../../../fs/fatfs/ff.h"
#include "data-structure/circular-queue.h"
#include "kernel/mtask.h"
#include <font.h>
#include <fs.h>
#include <kernel.h>
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

// typedef struct str_builder {
//   char  *str;
//   size_t len;
//   size_t cap;
// } *str_builder_t;

// str_builder_t str_builder_new(size_t cap) {
//   str_builder_t sb = malloc(sizeof(struct str_builder));
//   sb->str          = malloc(cap);
//   sb->len          = 0;
//   sb->cap          = cap;
//   return sb;
// }

// void str_builder_free(str_builder_t sb) {
//   free(sb->str);
//   free(sb);
// }

// void str_builder_append(str_builder_t sb, char ch) {
//   if (sb->len >= sb->cap) {
//     sb->cap *= 2;
//     sb->str  = realloc(sb->str, sb->cap);
//   }
//   sb->str[sb->len++] = ch;
// }

// void str_builder_insert(str_builder_t sb, int pos, char ch) {
//   if (sb->len >= sb->cap) {
//     sb->cap *= 2;
//     sb->str  = realloc(sb->str, sb->cap);
//   }
//   insert_char(sb->str, pos, ch);
//   sb->len++;
// }

// void str_builder_delete(str_builder_t sb, int pos) {
//   delete_char(sb->str, pos);
//   sb->len--;
// }

// void str_builder_clear(str_builder_t sb) {
//   sb->len = 0;
// }

// void str_builder_str(str_builder_t sb, char *str) {
//   strcpy(str, sb->str);
// }

ssize_t input(char *ptr, size_t len) {
  size_t p = 0;
  ptr[p]   = '\0';

  int c;
  while (p < len - 1 && (c = getch()) != '\n') {
    if (c == '\b') {
      if (p == 0) continue;
      p--;
      ptr[p] = '\0';
      auto x = get_x() - 1, y = get_y();
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

  return p;
}

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

void shell() {
  printi("shell has been started");
  void *kfifo = page_malloc_one();
  void *kbuf  = page_malloc_one();
  cir_queue8_init(kfifo, 0x1000, kbuf);
  current_task()->keyfifo = (cir_queue8_t)kfifo;
  char *path              = malloc(1024);
  char *ch                = malloc(255);
  sprintf(path, "/");
  while (true) {
    printf("%s# ", path);
    input(ch, 255);

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
    } else if (streq(ch, "ls")) {
      list_files(path);
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

  klogd("Set Mode");
  byte *vram = (void *)set_mode(1024, 768, 32);
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
  // auto font2 = load_font("/fatfs1/font2.plff");

  auto tty = plty_alloc(vram, 1024, 768, font1);
  // plty_addfont(tty, font2);

  plty_set_default(tty);

  // info("Plant-OS 终端现在支持中文啦！");

  // for (int i = 0;; i++) {
  //   info("Hello world! %d", i);
  // }

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
  int status = os_execute("/fatfs1/testapp.bin", "/fatfs1/testapp.bin 1 2 3");
  printi("TESTAPP.BIN exit with code %d", status);
  printi("%d alloced pages", page_get_alloced());

  infinite_loop task_next();
}
