#include <fs.h>

vfs_node_t rootdir = null;

static void empty_func() {}

struct vfs_callback vfs_empty_callback;

static vfs_callback_t fs_callbacks[256] = {
    [0] = &vfs_empty_callback,
};
static int fs_nextid = 1;

#define callbackof(node, _name_) (fs_callbacks[(node)->fsid]->_name_)

vfs_node_t vfs_node_alloc(vfs_node_t parent, cstr name);

finline char *pathtok(char **_rest sp) {
  char *s = *sp, *e = *sp;
  if (*s == '\0') return null;
  for (; *e != '\0' && *e != '/'; e++) {}
  *sp = e + (*e != '\0' ? 1 : 0);
  *e  = '\0';
  return s;
}

finline void do_open(vfs_node_t file) {
  if (file->handle != null) {
    callbackof(file, stat)(file->handle, file);
  } else {
    callbackof(file, open)(file->parent->handle, file->name, file);
  }
}

finline void do_update(vfs_node_t file) {
  assert(file->fsid != 0 || file->type != file_none);
  if (file->type == file_none) do_open(file);
  assert(file->type != file_none);
}

vfs_node_t vfs_child_append(vfs_node_t parent, cstr name, void *handle) {
  vfs_node_t node = vfs_node_alloc(parent, name);
  if (node == null) return null;
  node->handle = handle;
  return node;
}

static vfs_node_t vfs_child_find(vfs_node_t parent, cstr name) {
  return list_first(parent->child, data, streq(name, ((vfs_node_t)data)->name));
}

int vfs_mkdir(cstr name) {
  if (name[0] != '/') return -1;
  char      *path     = strdup(name + 1);
  char      *save_ptr = path;
  vfs_node_t current  = rootdir;
  for (char *buf = pathtok(&save_ptr); buf; buf = pathtok(&save_ptr)) {
    vfs_node_t father = current;
    current           = vfs_child_find(current, buf);
    if (current == null) {
      current       = vfs_node_alloc(father, buf);
      current->type = file_dir;
      callbackof(father, mkdir)(father->handle, buf, current);
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

int vfs_mkfile(cstr name) {
  if (name[0] != '/') return -1;
  char      *path     = strdup(name + 1);
  char      *save_ptr = path;
  vfs_node_t current  = rootdir;
  char      *filename = path + strlen(path);

  while (*--filename != '/' && filename != path) {}
  *filename++ = '\0';
  if (strlen(path) == 0) {
    free(path);
    return -1;
  }
  for (char *buf = pathtok(&save_ptr); buf; buf = pathtok(&save_ptr)) {
    current = vfs_child_find(current, buf);
    if (current == null || current->type != file_dir) goto err;
  }

  vfs_node_t node = vfs_child_append(current, filename, null);
  node->type      = file_block;
  callbackof(current, mkfile)(current->handle, filename, node);

  free(path);
  return 0;

err:
  free(path);
  return -1;
}

int vfs_regist(cstr name, vfs_callback_t callback) {
  if (callback == null) return -1;
  for (size_t i = 0; i < sizeof(struct vfs_callback) / sizeof(void *); i++) {
    if (((void **)callback)[i] == null) return -1;
  }
  int id           = fs_nextid++;
  fs_callbacks[id] = callback;
  return id;
}

vfs_node_t vfs_do_search(vfs_node_t dir, cstr name) {
  return list_first(dir->child, data, streq(name, ((vfs_node_t)data)->name));
}

vfs_node_t vfs_open(cstr str) {
  if (str == null) return null;
  if (str[1] == '\0') return rootdir;
  char *path = strdup(str + 1);
  if (path == null) return null;

  char      *save_ptr = path;
  vfs_node_t current  = rootdir;
  for (char *buf = pathtok(&save_ptr); buf; buf = pathtok(&save_ptr)) {
    vfs_node_t father = current;
    current           = vfs_child_find(current, buf);
    if (current == null) goto err;
    do_update(current);
  }

  free(path);
  return current;

err:
  free(path);
  return null;
}

bool vfs_init() {
  for (size_t i = 0; i < sizeof(struct vfs_callback) / sizeof(void *); i++) {
    ((void **)&vfs_empty_callback)[i] = empty_func;
  }

  rootdir       = vfs_node_alloc(null, null);
  rootdir->type = file_dir;
  return true;
}

void vfs_deinit() {
  // 目前并不支持
}

vfs_node_t vfs_node_alloc(vfs_node_t parent, cstr name) {
  vfs_node_t node = malloc(sizeof(struct vfs_node));
  if (node == null) return null;
  memset(node, 0, sizeof(struct vfs_node));
  node->parent = parent;
  node->name   = name ? strdup(name) : null;
  node->type   = file_none;
  node->fsid   = parent ? parent->fsid : 0;
  if (parent) list_prepend(parent->child, node);
  return node;
}

void vfs_free(vfs_node_t vfs) {
  if (vfs == null) return;
  list_free_with(vfs->child, (void (*)(void *))vfs_free);
  fs_callbacks[vfs->fsid]->close(vfs->handle);
  free(vfs->name);
  free(vfs);
}

int vfs_mount(cstr src, vfs_node_t node) {
  if (node == null) return -1;
  if (node->type != file_dir) return -1;
  void *handle = null;
  for (int i = 1; i < fs_nextid; i++) {
    if (fs_callbacks[i]->mount(src, node) == 0) {
      node->fsid = i;
      return 0;
    }
  }
  return -1;
}

int vfs_read(vfs_node_t file, void *addr, size_t offset, size_t size) {
  do_update(file);
  if (file->type != file_block) return -1;
  return callbackof(file, read)(file->handle, addr, offset, size);
}
