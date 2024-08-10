#include "../../../fs/fatfs/ff.h"
#include <font.h>
#include <fs.h>
#include <kernel.h>
#include <plty.h>

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
void insert_char(char *str, int pos, char ch);
void delete_char(char *str, int pos);
void input(char *ptr, int len) {
  /**
   * 别问我为什么这么写，我也不知道为什么
   * 将就用吧，能用就行
   */
  bzero(ptr, len);

  int i;
  // int BNOW = input_stack_get_now();
  // int NOW  = input_stack_get_now() - 1;
  // int rf = 0; //右方向键按下过吗？
  int Bmx, Bmy;
  Bmx = get_x();
  Bmy = get_y();
  for (i = 0; i != len; i++) {
    int in = getch();
    if (in == '\0') {
      i--;
      continue;
    }
    if (in == '\n') {
      ptr[strlen(ptr)] = 0;

      if (Bmy >= get_ysize() - 1) {
        int bx = get_x();
        int by = get_y();
        for (int j = 0; j < strlen(ptr) - i + 1; j++) {
          gotoxy(bx + j, by);
        }
        print("\n");
      } else {
        gotoxy(0, Bmy + (strlen(ptr) / get_xsize()) + 1);
      }
      // input_stack_set_now(BNOW);
      // if (i > 0) {
      //   input_stack_put(ptr);
      //   input_stack_set_now(BNOW - 1);
      // }

      return;
    } else if (in == '\b') {
      if (i == 0) {
        i--;
        continue;
      }
      if (ptr[i] == 0) {

        i--;
        ptr[i] = 0;
        i--;
        putchar('\b');
      } else {
        int bx = get_x();
        int by = get_y();
        i--;
        delete_char(ptr, i);
        i--;
        for (int j = 0; j < strlen(ptr) - i - 1; j++) {
          putchar(' ');
        }
        for (int j = 0; j < strlen(ptr) + 1; j++) {
          putchar('\b');
        }
        print(ptr);
        gotoxy(bx - 1, by);
      }
      continue;
    } else if (in == '\t') {
      //找到空格的扫描码
      // for (int k = 0; k < 0x54; ++k) {
      //   if (keytable1[k] == ' ') {
      //     // in = k;
      //     fifo8_put(task_get_key_fifo(current_task()), k);
      //     fifo8_put(task_get_key_fifo(current_task()), k);
      //     fifo8_put(task_get_key_fifo(current_task()), k);
      //     fifo8_put(task_get_key_fifo(current_task()), k);
      //     i--;
      //     break;
      //   }
      // }
      continue;
    } else if (in == -1) {
      // int bx = get_x();
      // int by = get_y();
      // if (get_free() != 1023 && NOW != 1023) //没有输入
      // {
      //   if (NOW < 1022) {
      //     NOW++;
      //     input_stack_set_now(NOW);
      //   }
      //   // printk("%d\n",get_free()==1023);
      //   for (int j = 0; j < strlen(ptr) - i; j++) {
      //     printchar(' ');
      //   }
      //   for (int j = 0; j < strlen(ptr); j++) {
      //     printchar('\b');
      //   }
      //   char *Str = input_stack_pop();
      //   clean(ptr, len);
      //   strcpy(ptr, Str);
      //   i = strlen(ptr);
      //   print(ptr);
      //   i--;

      // } else {
      //   // printk("get_free()==%d\n", get_free());
      //   i--;
      // }
      i--;
      continue;
    } else if (in == -2) {
      // int bx = get_x();
      // int by = get_y();
      // if (NOW != BNOW) {
      //   // rf = 1;
      //   NOW--;
      //   input_stack_set_now(NOW);

      //   for (int j = 0; j < strlen(ptr) - i; j++) {
      //     printchar(' ');
      //   }
      //   for (int j = 0; j < strlen(ptr); j++) {
      //     printchar('\b');
      //   }
      //   char *Str = input_stack_pop();
      //   clean(ptr, len);
      //   strcpy(ptr, Str);
      //   i = strlen(ptr);
      //   print(ptr);
      //   i--;
      //   continue;
      // }
      // i = strlen(ptr);
      i--;
      continue;
    } else if (in == -3) {
      if (i == 0) {
        i--;
        continue;
      }
      gotoxy(get_x() - 1, get_y());
      i -= 2;
      continue;
    } else if (in == -4) {
      if (ptr[i] == 0) {
        i--;
        continue;
      }
      gotoxy(get_x() + 1, get_y());
      continue;
    }
    if (ptr[i] == 0) {
      putchar(in);
      ptr[i] = in;
    } else {
      int bx = get_x(), by = get_y();
      int ry = get_raw_y();
      for (int j = 0; j < strlen(ptr) - i; j++) {
        putchar(' ');
      }
      if (bx == get_xsize()) bx = 0;

      int BMPX = get_x();
      int BMPY = get_y();
      for (int j = 0; j < strlen(ptr); j++) {
        putchar('\b');
      }
      insert_char(ptr, i, in);

      print(ptr);
      int NX  = get_x();
      int NY  = get_y();
      int RNy = get_raw_y();
      if (NY >= get_ysize() - 1) {
        if (BMPY == NY && NX <= BMPX) {
          gotoxy(bx + 1, by - (RNy - ry)); //还原到它应该在的地方
        } else {
          gotoxy(bx + 1, by);
        }
      } else {
        gotoxy(bx + 1, by);
      }
    }
  }
  ptr[strlen(ptr)] = 0; //设置终止符号0

  if (Bmy >= get_ysize() - 1) {
    int bx = get_x();
    int by = get_y();
    for (int j = 0; j < strlen(ptr) - i + 1; j++) {
      gotoxy(bx + j, by);
    }
    print("\n");
  } else {
    gotoxy(0, Bmy + (strlen(ptr) / get_xsize()) + 1);
  }
  // input_stack_set_now(BNOW);
  // if (i > 0) {
  //   input_stack_put(ptr);
  //   input_sstack_set_now(BNOW - 1); // Now-1
  // }
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
  char *kfifo = page_malloc_one();
  char *kbuf  = page_malloc_one();
  cir_queue_init(kfifo, 0x1000, kbuf);
  current_task()->keyfifo = (cir_queue_t)kfifo;
  char *path              = malloc(1024);
  char *ch                = malloc(255);
  sprintf(path, "/");
  for (;;) {
    // printi("%c", getch());
    // printf("psh 0.1 %s # ", path);
    // printf("psh 0.1\n");
    printf("%s# ", path);
    input(ch, 255);

    if (strneq(ch, "cd ", 3)) {
      char *s = ch + 3;
      if (streq(s, ".")) {
        continue;
      } else if (streq(s, "..")) {
        if (streq(s, "/")) continue;
        char *n = path + strlen(path);
        while (*--n != '/' && n != path) {}
        *n = '\0';
        if (strlen(path) == 0) strcpy(path, "/");
        continue;
      } else {
        if (streq(path, "/"))
          sprintf(path, "%s%s", path, s);
        else
          sprintf(path, "%s/%s", path, s);
      }
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

static void rand_str(char *s, size_t n) {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
  for (int i = 0; i < n; ++i) {
    s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }
  s[n] = '\0';
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
  // vfs_mkdir("/fatfs1/test");
  // vfs_mkfile("/fatfs1/test/a.txt");
  // scan_files("/");
  // vfs_mount_disk('B', 'B');
  // vfs_change_disk('B');
  // list_t l = vfs_listfile("");
  // printf("List files:");
  // list_foreach(l, file_node) {
  //   vfs_file *file = file_node->data;
  //   printf("name: %s  size: %d", file->name, file->size);
  // }

  auto font1 = load_font("/fatfs1/font1.plff");
  // auto font2 = load_font("/fatfs1/font2.plff");

  auto tty = plty_alloc(vram, 1024, 768, font1);
  // plty_addfont(tty, font2);

  for (int i = 0;; i++) {
    static char text[1024];
    static char buf[32];
    // sprintf(text, "你好，世界！  %d\n", i + 1);
    rand_str(buf, 16);
    sprintf(text, "Hello world!  %s %d\n", buf, i + 1);
    color_t color = {rand(), rand(), rand(), 255};
    plty_setfg(tty, color);
    plty_puts(tty, text);
    plty_flush(tty);
  }

  create_task((u32)shell, 0, 1, 1);
  create_task((u32)sound_test, 0, 1, 1);

  extern int init_ok_flag;
  init_ok_flag = 1;

  // printi("%d alloced pages", page_get_alloced());
  // int status = os_execute("TESTAPP.BIN", "./TESTAPP.BIN 1 2 3");
  // printi("TESTAPP.BIN exit with code %d", status);
  // printi("%d alloced pages", page_get_alloced());

  for (;;) {
    task_next();
  }
}
