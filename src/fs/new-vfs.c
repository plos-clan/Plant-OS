#include <fs.h>

vfs_node_t rootdir = null;

enum {
  file_none,   // 未获取信息
  file_dir,    // 文件夹
  file_block,  // 块设备，如硬盘
  file_stream, // 流式设备，如终端
};

static vfs_callback_t fs_callbacks[256];
static int            fs_nextid = 1;

#define callbackof(node) (fs_callbacks[(node)->fsid])

vfs_node_t vfs_node_alloc(vfs_node_t parent, cstr name);

finline char *pathtok(char **_rest sp) {
  char *s = *sp, *e = *sp;
  if (*s == '\0') return null;
  for (; *e != '\0' && *e != '/'; e++) {}
  *sp = e + (*e != '\0' ? 1 : 0);
  *e  = '\0';
  return s;
}

#define do_open(_file_)                                                                            \
  ({                                                                                               \
    if ((_file_)->handle != null) {                                                                \
      callbackof(_file_)->stat((_file_)->handle, _file_);                                          \
    } else {                                                                                       \
      callbackof(_file_)->open((_file_)->parent->handle, (_file_)->name, _file_);                  \
    }                                                                                              \
  })

#define do_update(_file_)                                                                          \
  ({                                                                                               \
    assert((_file_)->fsid != 0 || (_file_)->type != file_none);                                    \
    if ((_file_)->type == file_none) do_open(_file_);                                              \
    assert((_file_)->type != file_none);                                                           \
  })

int vfs_mkdir(cstr name) {
  if (name[0] != '/') return -1;
  char      *path     = strdup(name + 1);
  char      *save_ptr = path;
  vfs_node_t current  = rootdir;
  for (char *buf = pathtok(&save_ptr); buf; buf = pathtok(&save_ptr)) {
    vfs_node_t father = current;
    current           = list_first(current->child, data, streq(buf, ((vfs_node_t)data)->name));
    if (current == null) {
      current       = vfs_node_alloc(father, buf);
      current->type = file_dir;
      current->fsid = father->fsid;
      list_push(father->child, current);
      if (current->fsid > 0) callbackof(father)->mkdir(father->handle, buf, current);
    } else {
      do_update(current);
      if (current->type != file_dir) goto err;
    }
  }

  free(path);
  return 0;

err:
  free(path);
  return -1;
}

vfs_node_t vfs_child_append(vfs_node_t parent, cstr name, void *handle) {
  vfs_node_t node = vfs_node_alloc(parent, name);
  if (node == null) return null;
  node->handle = handle;
  list_prepend(parent->child, node);
  return node;
}

int vfs_mkfile(cstr name) {
  if (name[0] != '/') return -1;
  char      *path        = strdup(name + 1);
  char      *save_ptr    = path;
  vfs_node_t current_dir = rootdir;
  char      *filename    = path + strlen(path);

  while (*--filename != '/' && filename != path) {}
  *filename++ = '\0';

  for (char *buf = pathtok(&save_ptr); buf; buf = pathtok(&save_ptr)) {
    assert(current_dir->type != file_none);
    current_dir = list_first(current_dir->child, data, streq(buf, ((vfs_node_t)data)->name));
    if (!current_dir || current_dir->type != file_dir) {
      free(path);
      return -1;
    }
  }

  vfs_node_t node = vfs_child_append(current_dir, filename, null);
  node->type      = file_block;

  callbackof(current_dir)->mkfile(current_dir->handle, filename, node);
  free(path);
  return 0;
}

void vfs_regist(cstr name, vfs_callback_t callback) {
  if (callback == null) return;
  fs_callbacks[fs_nextid++] = callback;
}

vfs_node_t vfs_do_search(vfs_node_t dir, cstr name) {
  return list_first(dir->child, data, streq(name, ((vfs_node_t)data)->name));
}

vfs_node_t vfs_open(cstr str) {
  if (str == null) return null;
  char *path = strdup(str + 1);
  if (path == null) return null;

  char      *save_ptr    = path;
  vfs_node_t current_dir = rootdir;
  for (char *buf = pathtok(&save_ptr); buf; buf = pathtok(&save_ptr)) {
    vfs_node_t father = current_dir;
    current_dir       = list_first(current_dir->child, data, streq(buf, ((vfs_node_t)data)->name));
    if (current_dir == null) goto err;
    if (current_dir->type == file_none) do_open(current_dir);
  }

  free(path);
  return current_dir;

err:
  free(path);
  return null;
}

void tmpfs_regist();

bool vfs_init() {
  rootdir       = vfs_node_alloc(null, null);
  rootdir->type = file_dir;
  tmpfs_regist();
  vfs_mkdir("/tmpfs");
  vfs_node_t t = vfs_open("/tmpfs");
  t->fsid      = 1;
  vfs_mkfile("/tmpfs/a.txt");
  return true;
}

void vfs_deinit() {
  // 目前并不支持
}

vfs_node_t vfs_node_alloc(vfs_node_t parent, cstr name) {
  vfs_node_t vfs = malloc(sizeof(struct vfs_node));
  if (vfs == null) return null;
  memset(vfs, 0, sizeof(struct vfs_node));
  vfs->parent = parent;
  vfs->name   = name ? strdup(name) : null;
  vfs->type   = file_none;
  vfs->fsid   = parent ? parent->fsid : 0;
  return vfs;
}

void vfs_free(vfs_node_t vfs) {
  if (vfs == null) return;
  list_free_with(vfs->child, (void (*)(void *))vfs_free);
  fs_callbacks[vfs->fsid]->free(vfs->handle);
  free(vfs->name);
  free(vfs);
}
