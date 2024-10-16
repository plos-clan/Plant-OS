// This code is released under the MIT License

#include <font.h>
#include <fs.h>
#include <kernel.h>
#include <pl_readline.h>
#include <plty.h>

u8  *shell_data;
void ide_initialize(u32 BAR0, u32 BAR1, u32 BAR2, u32 BAR3, u32 BAR4);
int  os_execute(char *filename, char *line);

void idle_loop() {
  infinite_loop task_next();
}

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
  screen_flush();
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
void flush() {
  return;
}
void handle_tab(char *buf, pl_readline_words_t words) {
  pl_readline_word_maker_add("cd", words, true, ' ');
  pl_readline_word_maker_add("ls", words, true, ' ');
  pl_readline_word_maker_add("pcils", words, true, ' ');
  pl_readline_word_maker_add("exit", words, true, ' ');
  pl_readline_word_maker_add("clear", words, true, ' ');
  pl_readline_word_maker_add("read", words, true, ' ');
  pl_readline_word_maker_add("stdout_test", words, true, ' ');
  pl_readline_word_maker_add("mkdir", words, true, ' ');
  pl_readline_word_maker_add("mount", words, true, ' ');
  pl_readline_word_maker_add("file", words, true, ' ');
  pl_readline_word_maker_add("umount", words, true, ' ');

  if (buf[0] != '/' && strlen(buf)) { return; }
  char *s = malloc(strlen(buf) + 2);
  memcpy(s, buf, strlen(buf) + 1);
  if (strlen(s)) {
    for (int i = strlen(s); i >= 0; i--) {
      if (s[i] == '/') {
        s[i + 1] = '\0';
        break;
      }
    }
  } else {
    s[0] = '/';
    s[1] = '\0';
  }

  vfs_node_t p = vfs_open(s);
  if (!p) {
    free(s);
    return;
  }
  // 使用pl_readline_word_maker_add(char *word, pl_readline_words_t words, bool
  // is_first)

  // 添加words中的单词
  list_foreach(p->child, i) {
    vfs_node_t c        = (vfs_node_t)i->data;
    char      *new_path = pathcat(s, c->name);
    if (c->type == file_dir) {
      pl_readline_word_maker_add(new_path, words, false, '/');
    } else {
      pl_readline_word_maker_add(new_path, words, false, ' ');
    }
    free(new_path);
  }
  free(s);
}
void pci_list() {
  extern void *pci_addr_base;
  u8          *pci_drive = (u8 *)pci_addr_base;
  //输出PCI表的内容
  for (int line = 0;; pci_drive += 0x110 + 4, line++) {
    if (pci_drive[0] == 0xff)
      pci_classcode_print((struct pci_config_space_public *)(pci_drive + 12));
    else
      break;
  }
}

#include <magic.h>

static cstr filetype_from_name(cstr path) {
  auto file = vfs_open(path);
  if (file == null) return null;
  size_t size = min(1024, file->size);
  byte  *buf  = malloc(size);
  vfs_read(file, buf, 0, size);
  cstr type = filetype(buf, size);
  free(buf);
  return type;
}

static char *exec_name_from_cmdline(cstr line) {
  return parse_arg(null, &line);
}

void shell() {
  printi("shell has been started");
  void *kfifo = page_malloc_one();
  void *kbuf  = page_malloc_one();
  cir_queue8_init(kfifo, PAGE_SIZE, kbuf);
  current_task()->keyfifo = (cir_queue8_t)kfifo;
  char         *path      = malloc(1024);
  char         *ch        = malloc(255);
  pl_readline_t n;
  n = pl_readline_init(readline_getch, putchar, screen_flush, handle_tab);
  sprintf(path, "/");
  while (true) {
    char buf[255];
    sprintf(buf, "%s# ", path);
    pl_readline(n, buf, ch, 255);
    if (!strlen(ch)) continue;
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
    } else if (streq(ch, "pcils")) {
      pci_list();
    } else if (streq(ch, "exit")) {
      syscall_exit(0);
    } else if (streq(ch, "clear")) {
      screen_clear();
    } else if (streq(ch, "stdout_test")) {
      vfs_node_t stdout = vfs_open("/dev/stdout");
      vfs_write(stdout, "Hello, world!\n", 0, 14);
    } else if (strneq(ch, "file ", 5)) {
      cstr path = ch + 5;
      cstr type = filetype_from_name(path);
      if (type) {
        printf("%s\n", type);
      } else {
        printf("unknown file\n");
      }
    } else if (strneq(ch, "read ", 5)) {
      char      *s = ch + 5;
      vfs_node_t p = vfs_open(s);
      if (!p) {
        printf("open %s failed\n", s);
        continue;
      }
      if (p->type == file_dir) {
        printf("not a file\n");
        continue;
      }
      size_t size = p->size;
      byte  *buf  = malloc(size);
      vfs_read(p, buf, 0, size);
      printf("%s\n", buf);
      free(buf);
    } else if (strneq(ch, "mkdir ", 6)) {
      vfs_mkdir(ch + 6);
    } else if (strneq(ch, "mount ", 6)) {
      // 用strtok分割参数
      char *dev_name = strtok(ch + 6, " ");
      char *dir_name = strtok(null, " ");
      vfs_mount(dev_name, vfs_open(dir_name));
    } else if (strneq(ch, "umount ", 7)) {
      char *dir_name = ch + 7;
      int   ret      = vfs_unmount(dir_name);
      if (ret == -1) { printf("umount %s failed\n", dir_name); }

    } else {
      char       *path = exec_name_from_cmdline(ch);
      cstr        type = filetype_from_name(path);
      extern void plac_player(cstr path);
      extern void qoa_player(cstr path);
      if (streq(type, "application/x-executable")) {
        int status = os_execute(path, ch);
        printf("%s exited with code %d\n", path, status);
      } else if (streq(type, "audio/x-qoa")) {
        qoa_player(path);
      } else if (streq(type, "audio/x-plac")) {
        plac_player(path);
      } else {
        printf("bad command\n");
      }
      free(path);
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
void stdout_read() {}
void stdout_write(int drive, u8 *buffer, u32 number, u32 lba) {
  for (int i = 0; i < number; i++) {
    putchar(buffer[i]);
  }
}

char *GetSVGACharOEMString();
bool  is_vbox = false;
void  check_device() {
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

void draw(int n) {
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
  check_device();

  byte *vram = vbe_match_and_set_mode(screen_w, screen_h, 32);
  klogd("ok vram = %p", vram);
  memset(vram, 0, screen_w * screen_h * 4);

#if 1
  for (volatile size_t i = 0;; i++) {
    draw(i);
    vbe_flip();
  }
#endif

  floppy_init();
  ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
  vdisk vd;
  strcpy(vd.DriveName, "stdout");
  vd.flag        = 1;
  vd.sector_size = 1;
  vd.size        = 1;
  vd.Read        = (void *)stdout_read;
  vd.Write       = stdout_write;
  register_vdisk(vd);

  vfs_mkdir("/dev");
  vfs_mount(NULL, vfs_open("/dev"));

  vfs_mkdir("/fatfs1");
  vfs_mount("/dev/ide0", vfs_open("/fatfs1"));

  auto font1 = load_font("/fatfs1/font1.plff");
  // auto font2 = load_font("/fatfs1/font2.plff");

  auto tty = plty_alloc(vram, screen_w, screen_h, font1);
  // plty_addfont(tty, font2);

  plty_set_default(tty);

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

  // extern void sound_test();

  // extern void init_sound_mixer();
  // extern void sound_mixer_task();
  // extern void sound_test1();
  // extern void sound_test2();
  // init_sound_mixer();
  create_task(shell, 1, 1);
  // create_task(sound_test, 1, 1);
  // create_task(sound_mixer_task, 1, 1);
  // create_task(sound_test1, 1, 1);
  // create_task(sound_test2, 1, 1);

  infinite_loop task_next();
}
