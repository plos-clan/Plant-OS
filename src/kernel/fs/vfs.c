#include <fs.h>

vfs_t rootdir;

void vfs_regist_fs(cstr name, vfs_callback_t callback) {}

void vfs_open(cstr str);

bool vfs_init() {
  return true;
}

void vfs_deinit() {}
