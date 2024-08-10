#include "fatfs/ff.h"
#include <fs.h>
#include <kernel.h>
static FATFS volume[10];
static int   fatfs_id = 0;
typedef struct file {
  char *path;
  void *handle;
} *file_t;

int fatfs_mkdir(void *parent, cstr name, vfs_node_t node) {
  node->handle = null;
  return 0;
}

int fatfs_mkfile(void *parent, cstr name, vfs_node_t node) {}

int fatfs_readfile(file_t file, void *addr, size_t offset, size_t size) {
  if (file == null || addr == null) return -1;
  FRESULT res;
  res = f_lseek(file->handle, offset);
  if (res != FR_OK) return -1;
  u32 n;
  res = f_read(file->handle, addr, size, &n);
  if (res != FR_OK) return -1;
  return 0;
}

int fatfs_writefile(file_t file, const void *addr, size_t offset, size_t size) {
  return 0;
}

void fatfs_open(void *parent, cstr name, vfs_node_t node) {
  file_t p        = parent;
  char  *new_path = malloc(strlen(p->path) + strlen(name) + 1 + 1);
  file_t new      = malloc(sizeof(struct file));
  sprintf(new_path, "%s/%s", p->path, name);
  void   *fp = NULL;
  FILINFO fno;
  FRESULT res = f_stat(new_path, &fno);
  assert(res == FR_OK, "f_stat %d %s", res, new_path);
  if (fno.fattrib & AM_DIR) {
    //node.
    node->type = file_dir;
    fp         = malloc(sizeof(DIR));
    res        = f_opendir(fp, new_path);
    assert(res == FR_OK);
    for (;;) {
      //读取目录下的内容，再读会自动读下一个文件
      res = f_readdir(fp, &fno);
      //为空时表示所有项目读取完毕，跳出
      if (res != FR_OK || fno.fname[0] == 0) break;
      vfs_child_append(node, fno.fname, null);
    }
  } else {
    node->type = file_block;
    fp         = malloc(sizeof(FIL));
    res        = f_open(fp, new_path, FA_READ | FA_WRITE);
    node->size = f_size((FIL *)fp);
    assert(res == FR_OK);
  }

  assert(fp != NULL);
  new->handle  = fp;
  new->path    = new_path;
  node->handle = new;
}

void fatfs_close(file_t handle) {}

int fatfs_mount(cstr src, vfs_node_t node) {
  int drive  = *(int *)src;
  drive     -= 0x41;
  assert(drive < 10 && drive >= 0);
  char *path = malloc(3);
  bzero(path, 0);
  sprintf(path, "%d:", drive);
  FRESULT r = f_mount(&volume[drive], path, 1);
  if (r != FR_OK) {
    free(path);
    return -1;
  }
  file_t f = malloc(sizeof(struct file));
  f->path  = path;
  DIR *h   = malloc(sizeof(DIR));
  f_opendir(h, path);
  f->handle  = h;
  node->fsid = fatfs_id;

  FILINFO fno;
  FRESULT res;
  for (;;) {
    //读取目录下的内容，再读会自动读下一个文件
    res = f_readdir(h, &fno);
    //为空时表示所有项目读取完毕，跳出
    if (res != FR_OK || fno.fname[0] == 0) break;
    vfs_child_append(node, fno.fname, null);
  }
  node->handle = f;
  return 0;
}

void fatfs_unmount(void *root) {}

int dummy() {
  return 0;
}

static struct vfs_callback callbacks = {
    .mount   = fatfs_mount,
    .unmount = fatfs_unmount,
    .open    = fatfs_open,
    .close   = (vfs_close_t)fatfs_close,
    .read    = (vfs_read_t)fatfs_readfile,
    .write   = (vfs_write_t)fatfs_writefile,
    .mkdir   = fatfs_mkdir,
    .mkfile  = fatfs_mkfile,
    .stat    = (vfs_stat_t)dummy,
};

void fatfs_regist() {
  fatfs_id = vfs_regist("fatfs", &callbacks);
  klogd("%d", fatfs_id);
}