#include "../../../fs/fatfs/ff.h"
#include <font.h>
#include <fs.h>
#include <kernel.h>

void fatfs_regist();

u8  *shell_data;
void ide_initialize(u32 BAR0, u32 BAR1, u32 BAR2, u32 BAR3, u32 BAR4);
void sound_test();
int  os_execute(char *filename, char *line);

void idle() {
  for (;;) {
    task_next();
  }
}

void shell() {
  printi("shell has been started");
  char *kfifo = page_malloc_one();
  char *kbuf  = page_malloc_one();
  cir_queue_init(kfifo, 0x1000, kbuf);
  current_task()->keyfifo = (cir_queue_t)kfifo;
  for (;;) {
    printi("%c", getch());
  }
}

char *pathcat(cstr p1, cstr p2) {
  char *p = malloc(strlen(p1) + strlen(p2) + 2);
  if (p1[strlen(p1) - 1] == '/') {
    sprintf(p, "%s%s", p1, p2);
  } else {
    sprintf(p, "%s/%s", p1, p2);
  }
  return p;
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

void init() {
  klogd("init function has been called successfully!");
  printf("Hello Plant-OS!\n");

  klogd("Set Mode");
  byte *vram = (void *)set_mode(1024, 768, 32);
  memset(vram, 0xff, 1024 * 768 * 4);
  for (int i = 0; i < 20; i++) {
    printi("I=%d", i);
  }

  init_floppy();
  ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);

  fatfs_regist();
  int s = 0x41;
  vfs_mkdir("/fatfs0");
  vfs_mount(&s, vfs_open("/fatfs0"));
  s++;
  vfs_mkdir("/fatfs1");
  vfs_mount(&s, vfs_open("/fatfs1"));

  scan_files("/");
  vfs_mount_disk('B', 'B');
  vfs_change_disk('B');
  list_t l = vfs_listfile("");
  printf("List files:");
  list_foreach(l, file_node) {
    vfs_file *file = file_node->data;
    printf("name: %s  size: %d", file->name, file->size);
  }

  auto  file = vfs_open("/fatfs1/font.plff");
  byte *buf  = malloc(file->size);
  vfs_read(file, buf, 0, file->size);

  auto font = plff_load_from_mem(buf, file->size);
  klogi("font: %d %d %d", font->nchars, font->height, font->channels);
  u32 text[16];
  utf8_to_32s(text, "你好，世界");
  size_t bx = 50, by = 50;
  for (int i = 0; text[i]; i++) {
    auto ch = font_getchar(font, text[i]);
    for (int y = 0; y < ch->height; y++) {
      for (int x = 0; x < ch->width; x++) {
        size_t vx = bx + ch->left + x, vy = by + ch->top + y;
        vram[(vy * 1024 + vx) * 4 + 2] = 255 - ch->img[(y * ch->width + x) * 3];
        vram[(vy * 1024 + vx) * 4 + 1] = 255 - ch->img[(y * ch->width + x) * 3 + 1];
        vram[(vy * 1024 + vx) * 4]     = 255 - ch->img[(y * ch->width + x) * 3 + 2];
      }
    }
    bx += ch->advance;
  }

  create_task((u32)shell, 0, 1, 1);
  create_task((u32)sound_test, 0, 1, 1);

  extern int init_ok_flag;
  init_ok_flag = 1;
  klogd("set %d", init_ok_flag);

  printi("%d alloced pages", page_get_alloced());
  int status = os_execute("TESTAPP.BIN", "./TESTAPP.BIN 1 2 3");
  printi("TESTAPP.BIN exit with code %d", status);
  printi("%d alloced pages", page_get_alloced());

  for (;;) {
    task_next();
  }
}
