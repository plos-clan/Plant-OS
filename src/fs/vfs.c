#include <fs.h>
/*
/mnt/testfs0/a.txt
 */
vfs_t rootdir;
/**
文件系统需要支持的函数:
open
read
write
close

createf
removef

opendir
next
closedir

createdir
removedir
 */

enum {
  file_dir,    // 文件夹
  file_block,  // 块设备，如硬盘
  file_stream, // 流式设备，如终端
};

int vfs_mkdir(cstr name) {}

void vfs_regist_fs(cstr name, vfs_callback_t callback) {}

vfs_t vfs_open(cstr str) {
  if (str == null) return null;
  char *path = strdup(str);
  if (path == null) return null;

  if (*path == '\0') {}

  return null;
}

vfs_t vfs_opendir(cstr path) {}

int vfs_dir_next(vfs_t dir, void *p) {}

bool vfs_init() {
  return true;
}

void vfs_deinit() {}
