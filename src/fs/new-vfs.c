#include <fs.h>
/*
cd /mnt/testfs0
../testfs1/a.txt

cd /
cd /mnt/testfs0/13123/../../../bin/sh
 */
vfs_nmsb_t rootdir = null;
/**
文件系统需要支持的函数:
open
read
write
close

createf
removef

opendir(void *dir,char *name)
next
closedir

createdir
removedir

get_root_dir
 */

enum {
  file_dir,    // 文件夹
  file_block,  // 块设备，如硬盘
  file_stream, // 流式设备，如终端
};

static vfs_callback_t fs_callbacks[256];
static int            fs_nextid = 1;

vfs_nmsb_t vfs_alloc();

int vfs_mkdir(cstr name) {
  if (name[0] != '/') return -1;
  char      *path = strdup(name + 1);
  char      *save_ptr;
  char      *buf         = strtok_r(path, "/", &save_ptr);
  vfs_nmsb_t current_dir = rootdir;
  while (buf) {
    vfs_nmsb_t father = current_dir;
    current_dir       = list_first(current_dir->child, data, streq(buf, ((vfs_nmsb_t)data)->name));
    if (!current_dir || current_dir->type != file_dir) {
      current_dir         = vfs_alloc();
      current_dir->parent = father;
      current_dir->type   = file_dir;
      current_dir->name   = strdup(buf);
      current_dir->child  = null;
      list_push(father->child, current_dir);
      if (father->fsid) {
      _1:
        if (father->handle) {
          fs_callbacks[father->fsid]->mkdir(father->handle, buf);
        } else {
          father->handle =
              fs_callbacks[father->fsid]->opendir(father->parent->handle, father->name);
          goto _1;
        }
      }
    }
    buf = strtok_r(null, "/", &save_ptr);
  }

  free(path);
  return 0;
}

void vfs_regist(cstr name, vfs_callback_t callback) {
  if (callback == null) return;
  fs_callbacks[fs_nextid++] = callback;
}

vfs_nmsb_t vfs_open(cstr str) {
  if (str == null) return null;
  char *path = strdup(str);
  if (path == null) return null;

  if (*path == '\0') {}

  return null;
}

vfs_nmsb_t vfs_opendir(cstr path) {}

int vfs_dir_next(vfs_nmsb_t dir, void *p) {}

bool vfs_init() {
  rootdir       = vfs_alloc();
  rootdir->name = strdup("/");
  rootdir->fsid = 0;
  vfs_mkdir("/mnt");
  return true;
}

void vfs_deinit() {}

vfs_nmsb_t vfs_alloc() {
  vfs_nmsb_t vfs = malloc(sizeof(struct vfs_nmsb));
  if (vfs == null) return null;
  vfs->parent = null;
  vfs->name   = null;
  return vfs;
}

void vfs_free(vfs_nmsb_t vfs) {
  if (vfs == null) return;
  list_free_with(vfs->child, (void (*)(void *))vfs_free);
  fs_callbacks[vfs->fsid]->free(vfs->handle);
  free(vfs->name);
  free(vfs);
}
