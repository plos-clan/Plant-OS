// This code is released under the MIT License

#include <fs.h>
#include <kernel.h>
#include <magic.h>
#include <pl_readline.h>
#include <plty.h>

static void list_files(cstr path) {
  klogd("%s", path);
  val dir = vfs_open(path);
  assert(dir, "open %s failed", path);
  assert(dir->type == file_dir);
  list_foreach(dir->child, node) {
    val file = (vfs_node_t)node->data;
    printf("%s\t ", file->name);
  }
  printf("\n");
}

static int readline_getch() {
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

static void handle_tab(cstr buf, pl_readline_words_t words) {
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
    for (isize i = strlen(s); i >= 0; i--) {
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

static void pci_list() {
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

static bool isprint(char c) {
  return c >= 0x20 && c <= 0x7e;
}

static void print_data(const byte *data, usize len) {
  for (usize i = 0; i < len; i += 16) {
    printf("\033[1;33m%08x\033[0m  ", i);
    for (usize j = 0; j < 16; j++) {
      if (i + j < len) {
        printf("%02x ", data[i + j]);
      } else {
        printf("   ");
      }
    }
    printf(" ");
    for (usize j = 0; j < 16; j++) {
      if (i + j >= len) break;
      if (isprint(data[i + j]))
        printf("\033[1;32m%c\033[0m", data[i + j]);
      else
        printf("\033[1;30m.\033[0m");
    }
    printf("\n");
  }
}

static int print_file(cstr path) {
  vfs_node_t file = vfs_open(path);
  if (file == null) {
    printf("open %s failed\n", path);
    return 1;
  }
  if (file->type != file_block && file->type != file_stream) {
    printf("not a file\n");
    return 1;
  }
  if (file->size > 1024 * 1024) {
    printf("file too large\n");
    return 1;
  }

  printf("\033[1;35mHEX DUMP\033[0m  ");
  for (usize j = 0; j < 16; j++) {
    printf("\033[1;37m%02x\033[0m ", j);
  }
  printf(" \033[1;36mCHAR\033[0m\n");

  if (file->type == file_block) {
    byte *buf = malloc(file->size);
    vfs_read(file, buf, 0, file->size);
    print_data(buf, file->size);
    free(buf);
  } else {
    byte *buf = malloc(1024);
    while (true) {
      val len = vfs_read(file, buf, 0, 1024);
      if (len == 0) break;
      print_data(buf, len);
    }
    free(buf);
  }

  return 0;
}

static int shell_exec(char *path, cstr command) {
  if (!strlen(command)) return 0;
  int retcode = 0;
  if (strneq(command, "echo ", 5)) {
    printf("%s\n", command + 5);
  } else if (strneq(command, "cd ", 3)) {
    char *s = command + 3;
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
    if (vfs_open(path) == null || vfs_open(path)->type != file_dir) {
      printf("cd: %s: No such directory\n", s);
      sprintf(path, "%s", old);
      free(old);
      return 1;
    }
    free(old);
  } else if (streq(command, "ls")) {
    list_files(path);
  } else if (streq(command, "pcils")) {
    pci_list();
  } else if (streq(command, "exit")) {
    syscall_exit(0);
  } else if (streq(command, "clear")) {
    screen_clear();
  } else if (streq(command, "stdout_test")) {
    vfs_node_t stdout = vfs_open("/dev/stdout");
    vfs_write(stdout, "Hello, world!\n", 0, 14);
  } else if (strneq(command, "file ", 5)) {
    cstr type = filetype_from_name(command + 5);
    if (type) {
      printf("%s\n", type);
    } else {
      printf("unknown file\n");
    }
  } else if (strneq(command, "read ", 5)) {
    return print_file(command + 5);
  } else if (strneq(command, "mkdir ", 6)) {
    vfs_mkdir(command + 6);
  } else if (strneq(command, "mount ", 6)) {
    // 用strtok分割参数
    cstr dev_name = strtok(command + 6, " ");
    cstr dir_name = strtok(null, " ");
    vfs_mount(dev_name, vfs_open(dir_name));
  } else if (strneq(command, "umount ", 7)) {
    cstr dir_name = command + 7;
    if (vfs_unmount(dir_name) < 0) {
      printf("umount %s failed\n", dir_name);
      return 1;
    }
  } else if (strneq(command, "test ", 5)) {
    vfs_node_t n = vfs_open(command + 5);
    if(n) {
      char *path = vfs_get_fullpath(n);
      printf("%s\n", path);
      free(path);
    }
  }  else {
    char *path = exec_name_from_cmdline(command);
    cstr  type = filetype_from_name(path);
    if (streq(type, "application/x-executable")) {
      int status = os_execute(path, command);
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

void autorun() {
  var file = vfs_open("/fatfs0/autorun.txt");
  if (file == null) return;
  size_t size = file->size;
  char  *buf  = malloc(size + 1);
  vfs_read(file, buf, 0, size);
  char *p = buf;
  while (p < buf + size) {
    const char *line = p;
    while (*p != '\n' && p < buf + size) {
      p++;
    }
    *p = '\0';
    if (line != p) {
      printf("# %s\n", line);
      shell_exec("/", line);
    }
    p++;
  }
  task_exit(0);
}

void shell() {
  printi("shell has been started");
  cir_queue8_t kfifo = malloc(sizeof(struct cir_queue8));
  void        *kbuf  = page_malloc_one();
  cir_queue8_init(kfifo, PAGE_SIZE, kbuf);
  current_task->keyfifo = kfifo;
  char *path            = malloc(1024);
  val   readln          = pl_readline_init(readline_getch, putchar, screen_flush, handle_tab);
  sprintf(path, "/");
  char prompt[256];
  while (true) {
    sprintf(prompt, "\033[1;32mroot\033[m:\033[1;36m%s\033[m#\033[1;33m ", path);
    cstr line = pl_readline(readln, prompt);
    printf("\033[m");
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
