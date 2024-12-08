// This code is released under the MIT License

#include <fs.h>
#include <kernel.h>
#include <magic.h>
#include <pl_readline.h>
#include <plty.h>

int os_execute(char *filename, char *line);

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
    printf("%s\t ", c->name);
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
    vfs_update(c);
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
    if (pci_drive[0] != 0xff) break;
    pci_classcode_print((struct pci_config_space_public *)(pci_drive + 12));
  }
}

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
      if (s[strlen(s) - 1] == '/' && strlen(s) > 1) { s[strlen(s) - 1] = '\0'; }
      if (streq(s, ".")) continue;
      if (streq(s, "..")) {
        if (streq(s, "/")) continue;
        char *n = path + strlen(path);
        while (*--n != '/' && n != path) {}
        *n = '\0';
        if (strlen(path) == 0) strcpy(path, "/");
        continue;
      }
      char *old = strdup(path);
      if (s[0] == '/') {
        strcpy(path, s);
      } else {
        if (streq(path, "/"))
          sprintf(path, "%s%s", path, s);
        else
          sprintf(path, "%s/%s", path, s);
      }
      if (vfs_open(path) == null) {
        printf("cd: %s: No such directory\n", s);
        sprintf(path, "%s", old);
        free(old);
        continue;
      }
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
      byte  *buf  = malloc(1024);
      size_t pos  = 0;
      while (1) {
        size_t len = vfs_read(p, buf, pos, 1024);
        if (len == 0) break;
        for (int i = 0; i < len; i++) {
          printf("%02x ", buf[i]);
          if ((i + 1) % 32 == 0) printf("\n");
        }
        pos += len;
      }
      free(buf);
    } else if (strneq(ch, "readhex ", 8)) {
      char      *s = ch + 8;
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
      memset(buf, 0, size);
      vfs_read(p, buf, 0, size);
      for (size_t i = 0; i < size; i++) {
        printf("%02x ", buf[i]);
      }
      printf("\n");
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
      void        mp3_player(cstr path);
      if (streq(type, "application/x-executable")) {
        int status = os_execute(path, ch);
        printf("%s exited with code %d\n", path, status);
      } else if (streq(type, "audio/x-qoa")) {
        qoa_player(path);
      } else if (streq(type, "audio/x-plac")) {
        plac_player(path);
      } else if (streq(type, "audio/mpeg")) {
        mp3_player(path);
      } else {
        printf("bad command\n");
      }
      free(path);
    }
  }
}
