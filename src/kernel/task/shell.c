// This code is released under the MIT License

#include <fs.h>
#include <kernel.h>
#include <magic.h>
#include <pl_readline.h>
#include <plty.h>

int os_execute(char *filename, char *line);

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

extern void plac_player(cstr path);
extern void qoa_player(cstr path);
extern void mp3_player(cstr path);

int shell_exec(char *path, cstr comand) {
  if (!strlen(comand)) return 0;
  int retcode = 0;
  if (strneq(comand, "cd ", 3)) {
    char *s = comand + 3;
    if (s[strlen(s) - 1] == '/' && strlen(s) > 1) { s[strlen(s) - 1] = '\0'; }
    if (streq(s, ".")) return 0;
    if (streq(s, "..")) {
      if (streq(s, "/")) return 1;
      char *n = path + strlen(path);
      while (*--n != '/' && n != path) {}
      *n = '\0';
      if (strlen(path) == 0) strcpy(path, "/");
      return 0;
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
      return 1;
    }
  } else if (streq(comand, "ls")) {
    list_files(path);
  } else if (streq(comand, "pcils")) {
    pci_list();
  } else if (streq(comand, "exit")) {
    syscall_exit(0);
  } else if (streq(comand, "clear")) {
    screen_clear();
  } else if (streq(comand, "stdout_test")) {
    vfs_node_t stdout = vfs_open("/dev/stdout");
    vfs_write(stdout, "Hello, world!\n", 0, 14);
  } else if (strneq(comand, "file ", 5)) {
    cstr path = comand + 5;
    cstr type = filetype_from_name(path);
    if (type) {
      printf("%s\n", type);
    } else {
      printf("unknown file\n");
    }
  } else if (strneq(comand, "read ", 5)) {
    char      *s = comand + 5;
    vfs_node_t p = vfs_open(s);
    if (!p) {
      printf("open %s failed\n", s);
      return 1;
    }
    if (p->type == file_dir) {
      printf("not a file\n");
      return 1;
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
  } else if (strneq(comand, "readhex ", 8)) {
    char      *s = comand + 8;
    vfs_node_t p = vfs_open(s);
    if (!p) {
      printf("open %s failed\n", s);
      return 1;
    }
    if (p->type == file_dir) {
      printf("not a file\n");
      return 1;
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
  } else if (strneq(comand, "mkdir ", 6)) {
    vfs_mkdir(comand + 6);
  } else if (strneq(comand, "mount ", 6)) {
    // 用strtok分割参数
    char *dev_name = strtok(comand + 6, " ");
    char *dir_name = strtok(null, " ");
    vfs_mount(dev_name, vfs_open(dir_name));
  } else if (strneq(comand, "umount ", 7)) {
    char *dir_name = comand + 7;
    int   ret      = vfs_unmount(dir_name);
    if (ret == -1) { printf("umount %s failed\n", dir_name); }
  } else {
    char *path = exec_name_from_cmdline(comand);
    cstr  type = filetype_from_name(path);
    if (streq(type, "application/x-executable")) {
      int status = os_execute(path, comand);
      printf("%s exited with code %d\n", path, status);
    } else if (streq(type, "audio/x-qoa")) {
      qoa_player(path);
    } else if (streq(type, "audio/x-plac")) {
      plac_player(path);
    } else if (streq(type, "audio/mpeg")) {
      mp3_player(path);
    } else {
      printf("bad command\n");
      retcode = 1;
    }
    free(path);
  }
  return retcode;
}

void shell() {
  printi("shell has been started");
  void *kfifo = page_malloc_one();
  void *kbuf  = page_malloc_one();
  cir_queue8_init(kfifo, PAGE_SIZE, kbuf);
  current_task->keyfifo = (cir_queue8_t)kfifo;
  char         *path    = malloc(1024);
  pl_readline_t n;
  n = pl_readline_init(readline_getch, putchar, screen_flush, handle_tab);
  sprintf(path, "/");
  char prompt[256];
  while (true) {
    sprintf(prompt, "%s# ", path);
    cstr line = pl_readline(n, prompt);
    shell_exec(path, line);
  }
}

struct event debug_shell_event;
char         debug_shell_path[1024];

extern void log_update();

extern char debug_shell_message[256];

static const struct {
  cstr alias;
  cstr command;
} debug_commands[] = {
    {"test", "/fatfs1/testapp.bin"  },
    {"zstd", "/fatfs1/zstd-test.bin"},
    {"plui", "/fatfs1/plui-app.bin" },
    {"pf",   "/fatfs1/pf.bin"       },
};

void debug_shell() {
  sprintf(debug_shell_path, "/");
  log_update();
  while (true) {
    char *line = event_pop(&debug_shell_event);
    if (line == null) {
      task_next();
      continue;
    }
    if (line[0] == '\0') {
      strcpy(debug_shell_message, "Empty command");
    } else {
      strcpy(debug_shell_message, "Executing command");
      log_update();
      int  retcode;
      bool is_alias = false;
      for (int i = 0; i < lengthof(debug_commands); i++) {
        if (streq(line, debug_commands[i].alias)) {
          retcode  = shell_exec(debug_shell_path, debug_commands[i].command);
          is_alias = true;
          break;
        }
      }
      if (!is_alias) retcode = shell_exec(debug_shell_path, line);
      sprintf(debug_shell_message, "Command exited with code %d", retcode);
    }
    log_update();
    free(line);
  }
}
