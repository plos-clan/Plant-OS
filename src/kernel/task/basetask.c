#include <kernel.h>

void idle() {
  for (;;) {
    asm_hlt;
  }
}
void shell() {
  printi("shell has been started");
  char *kfifo = page_malloc_one();
  char *kbuf  = page_malloc_one();
  circular_queue_init(kfifo, 0x1000, kbuf);
  current_task()->keyfifo = (circular_queue_t)kfifo;
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
  int d = 0;
  list_foreach(l, file_node) {
    vfs_file *file = file_node->data;
    printf("%s ", file->name);
    d = file->size;
  }
  u8 *buf = malloc(d);
  vfs_readfile("kernel.bin", buf);
  for (int i = 0; i < 0x200; i++) {
    printf("%c", buf[i]);
  }

  create_task((u32)shell, 0, 1, 1);
  for (;;) {}
}
