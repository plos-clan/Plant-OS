#include "../../../fs/fatfs/ff.h"
#include <kernel.h>

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
static FRESULT scan_files(char *path) {
  FRESULT res; //部分在递归过程被修改的变量，不用全局变量
  FILINFO fno;
  DIR     dir;
  int     i;
  char   *fn; // 文件名

  /* 长文件名支持 */
  /* 简体中文需要2个字节保存一个“字”*/

  //打开目录
  res = f_opendir(&dir, path);
  if (res == FR_OK) {
    i = strlen(path);
    for (;;) {
      //读取目录下的内容，再读会自动读下一个文件
      res = f_readdir(&dir, &fno);
      //为空时表示所有项目读取完毕，跳出
      if (res != FR_OK || fno.fname[0] == 0) break;
      fn = fno.fname;
      //点表示当前目录，跳过
      if (*fn == '.') continue;
      //目录，递归读取
      if (fno.fattrib & AM_DIR) {
        //合成完整目录名
        sprintf(&path[i], "/%s", fn);
        //递归遍历
        res     = scan_files(path);
        path[i] = 0;
        //打开失败，跳出循环
        if (res != FR_OK) break;
      } else {
        printf("%s/%s\r\n", path, fn); //输出文件名
                                       /* 可以在这里提取特定格式的文件路径 */
      }                                //else
    }                                  //for
  }
  return res;
}
void init() {
  klogd("init function has been called successfully!");
  printf("Hello Plant-OS!\n");

  // klogd("Set Mode");
  // u32 *vram = (void *)set_mode(1024, 768, 32);
  // memset(vram, 0xff, 1024 * 768 * 4);
  // for (int i = 0; i < 20; i++) {
  //   printi("I=%d", i);
  // }

  init_floppy();
  ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
  FATFS   fs, fs1;
  FRESULT r = f_mount(&fs, "0:", 1);
  if (r == FR_OK) { printi("OK!"); }

  r = f_mount(&fs1, "1:", 1);
  if (r == FR_OK) {
    printi("OK!");
  } else {
    fatal("wcnm %d", r);
  }
  scan_files("0:");
  scan_files("1:");
  FIL fp;
  f_open(&fp, "1:testapp.bin", FA_READ);
  char *buff;
  buff = malloc(f_size(&fp));
  u32 n;
  f_read(&fp, buff, f_size(&fp), &n);
  printf("%s", buff);

  vfs_mount_disk('B', 'B');
  vfs_change_disk('B');
  list_t l = vfs_listfile("");
  printf("List files:");
  list_foreach(l, file_node) {
    vfs_file *file = file_node->data;
    printf("name: %s  size: %d", file->name, file->size);
  }
  // byte *buf = malloc(vfs_filesize("kernel.bin"));
  // vfs_readfile("kernel.bin", buf);

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
