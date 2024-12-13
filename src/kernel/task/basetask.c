// This code is released under the MIT License

#include <font.h>
#include <fs.h>
#include <kernel.h>
#include <pl_readline.h>
#include <plty.h>

void ide_initialize(u32 BAR0, u32 BAR1, u32 BAR2, u32 BAR3, u32 BAR4);
int  os_execute(char *filename, char *line);
void v86_task();
void shell();
void debug_shell();

static void scan_files(char *path) {
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

static plff_t load_font(cstr path) {
  auto  file = vfs_open(path);
  byte *buf  = malloc(file->size);
  vfs_read(file, buf, 0, file->size);
  auto font = plff_load_from_mem(buf, file->size);
  klogi("font %s: %d %d %d", path, font->nchars, font->height, font->channels);
  free(buf);
  return font;
}

void plty_set_default(plty_t plty);
void stdout_read() {}
void stdout_write(int drive, u8 *buffer, u32 number, u32 lba) {
  for (int i = 0; i < number; i++) {
    putchar(buffer[i]);
  }
}
void random_read(int drive, u8 *buffer, u32 number, u32 lba) {
  for (int i = 0; i < number; i++) {
    buffer[i] = rand() % 256;
  }
}
void  random_write(int drive, u8 *buffer, u32 number, u32 lba) {}
char *GetSVGACharOEMString();
bool  is_vbox = false;

static void check_device() {
  char *s = strdup(GetSVGACharOEMString());
  info("VIDEO CARD: %s", s);
  if (strstr(s, "VirtualBox")) {
    info("VirtualBox detected");
    is_vbox = true;
  } else {
    is_vbox = false;
    info("VirtualBox not detected");
  }
  free(s);
}

static void draw(int n) {
  u32 *buf = vbe_backbuffer;
  for (size_t y = 0; y < screen_h; y++) {
    for (size_t x = 0; x < screen_w; x++) {
      buf[y * screen_w + x] = (0xff8000 + x + n) & 0xffffff;
    }
  }
}

void init() {
  klogd("init function has been called successfully!");
  printf("Hello Plant-OS!\n");
  vfs_mkdir("/dev");
  vfs_mount(NULL, vfs_open("/dev"));
  floppy_init();
  ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
  vdisk vd;
  strcpy(vd.drive_name, "stdout");
  vd.flag        = 1;
  vd.sector_size = 1;
  vd.size        = 1;
  vd.read        = (void *)stdout_read;
  vd.write       = stdout_write;
  vd.type        = VDISK_STREAM;
  regist_vdisk(vd);
  srand(system_tick);
  vdisk rnd;
  strcpy(rnd.drive_name, "random");
  rnd.flag        = 1;
  rnd.sector_size = 1;
  rnd.size        = 1;
  rnd.read        = (void *)random_read;
  rnd.write       = random_write;
  rnd.type        = VDISK_STREAM;
  regist_vdisk(rnd);
  vfs_mkdir("/fatfs1");
  vfs_mount("/dev/ide0", vfs_open("/fatfs1"));

  vfs_mkdir("/fatfs2");
  vfs_mount("/dev/floppy", vfs_open("/fatfs2"));

  // vfs_node_t p = vfs_open("/dev/stdout");
  // assert(p, "open /dev/stdout failed");
  // while(1) vfs_write(p, "你好，世界", 0, strlen("你好，世界"));
  // infinite_loop;

  // const int bx = 200, by = 200, r = 100;
  // for (int y = 0; y < screen_w; y++) {
  //   for (int x = 0; x < screen_h; x++) {
  //     struct __PACKED__ {
  //       byte b, g, r, a;
  //     } *const buf = (void *)vram;
  //     if ((x - bx) * (x - bx) + (y - by) * (y - by) >= (r - 1) * (r - 1) &&
  //         (x - bx) * (x - bx) + (y - by) * (y - by) < r * r) {
  //       buf[y * screen_w + x].r = 255;
  //       buf[y * screen_w + x].g = 255;
  //       buf[y * screen_w + x].b = 255;
  //     } else {
  //       buf[y * screen_w + x].r = 0;
  //       buf[y * screen_w + x].g = 0;
  //       buf[y * screen_w + x].b = 0;
  //     }
  //   }
  // }

  screen_clear();
  task_t t = create_task(v86_task, 1, 1);
  check_device();
  u32 *vram = vbe_match_and_set_mode(screen_w, screen_h, 32);
  klogd("ok vram = %p", vram);
  lgmemset32(vram, 0, screen_w * screen_h);

#if 0 // 尝试 os-terminal 库
  void terminal_init(int width, int height, u32 *screen, void *(*malloc)(size_t size),
                     void (*free)(void *), void (*serial)(const char *s));
  void terminal_destroy();
  void terminal_flush();
  void terminal_advance_state_single(char c);
  void terminal_advance_state(char *s);
  void terminal_set_auto_flush(unsigned int auto_flush);

  terminal_init(screen_w, screen_h, vram, malloc, free, klog_raw);
  terminal_set_auto_flush(true);

  terminal_advance_state("Hello world!\n");
  terminal_advance_state("你好，世界！\n");

  infinite_loop;
#endif

#if 0
  for (volatile size_t i = 0;; i++) {
    draw(i);
    vbe_flip();
  }
#endif

  var font1 = load_font("/fatfs1/font1.plff");
  // var font2 = load_font("/fatfs1/font2.plff");

  var tty = plty_alloc(vram, screen_w, screen_h, font1);
  // plty_addfont(tty, font2);

#if 0
  char s[128];
  for (size_t i = 0;; i++) {
    sprintf(s,
            i & 1 ? "Hello, Plant-OS! A simple OS used to study made by several students.  x%08d\n"
                  : " Hello, Plant-OS! A simple OS used to study made by several students. x%08d\n",
            i);
    plty_puts(tty, s);
    plty_flush(tty);
  }
#endif
  void task_kill(u32 tid);
  plty_set_default(tty);
  task_kill(t->tid);

  create_task(shell, 1, 1);
  create_task(debug_shell, 1, 1);

  extern bool debug_enabled;
  debug_enabled = true;

  infinite_loop task_next();
}
