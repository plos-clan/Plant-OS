#include <kernel.h>
u8  *shell_data;
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

void init() {
  logd("init function has been called successfully!");
  printf("Hello Plant-OS!\n");
  // logd("Set Mode");
  // u32 *vram = (void *)set_mode(1024, 768, 32);
  // memset(vram, 0xff, 1024 * 768 * 4);
  // for (int i = 0; i < 20; i++) {
  //   printi("I=%d", i);
  // }
  init_floppy();
  vfs_mount_disk(current_task()->drive, current_task()->drive);
  vfs_change_disk(current_task()->drive);
  list_t l = vfs_listfile("");
  printf("List files:");
  list_foreach(l, file_node) {
    vfs_file *file = file_node->data;
    printf("name: %s  size: %d", file->name, file->size);
  }
  byte *buf = malloc(vfs_filesize("kernel.bin"));
  vfs_readfile("kernel.bin", buf);

  create_task((u32)shell, 0, 1, 1);
  create_task((u32)sound_test, 0, 1, 1);

  extern int init_ok_flag;
  init_ok_flag = 1;
  logd("set %d", init_ok_flag);

  int status = os_execute("TESTAPP.BIN", "");
  printi("TESTAPP.BIN exit with code %d", status);
  for (;;) {
    task_next();
  }
}
