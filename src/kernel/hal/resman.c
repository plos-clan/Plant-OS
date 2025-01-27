// This code is released under the MIT License

#include <kernel.h>

static describtor_t descriptor_open(char *path);
st_t                descriptors;

static i32 alloc_next_id(resman_t man) {
  while (avltree_get(man->avltree, man->next_id++))
    ;
  return man->next_id - 1;
}

void resman_descriptors_init() {
  descriptors = st_new();
  descriptor_open("/"); // 防止根目录被关闭
}

// **NOTE**: the path is a absolute path, not a relative path
static describtor_t descriptor_open(char *path) {
  // 首先，我们要看看能不能命中符号表
  describtor_t fd = st_get(descriptors, path);
  if (fd) {
    fd->refrence_cout++;
    return fd;
  }
  // 如果没有命中，那么我们就要打开文件
  vfs_node_t node = vfs_open(path);
  if (!node) return null; // 打开失败
  // 创建文件描述符
  fd                = malloc(sizeof(struct descriptor));
  fd->node          = node;
  fd->refrence_cout = 1;
  st_insert(descriptors, path, fd);
  return fd;
}
// @return: -1 works failed, 0 works successfully
static int descriptor_close(describtor_t fd) {
  if (!fd) return -1;
  if (--fd->refrence_cout == 0) {
    vfs_close(fd->node);
    st_remove(descriptors, vfs_get_fullpath(fd->node));
    free(fd);
  }
  return 0;
}

resman_t resman_alloc() {
  resman_t man = malloc(sizeof(struct resman));
  resman_init(man);
  return man;
}

void resman_init(resman_t man) {
  if (man == null) return;
  man->avltree = null;
  man->next_id = 0;
}

// @return: id ,if -1 it works failed
int resman_open(resman_t man, i32 working_dir, cstr path) {
  describtor_t fd_dir;
  vfs_node_t   dir;
  char        *full_path = null;

  if (path[0] == '/') {
    dir = vfs_open("/");
    assert(dir, "open / failed");
    full_path = strdup(path);
  } else {
    fd_dir = avltree_get(man->avltree, working_dir);
    klogd("fd_dir is %p", fd_dir);
    if (!fd_dir) return -1; // 无效的工作目录
    dir = fd_dir->node;
    if (dir->type != file_dir) return -1; // 工作目录不是一个目录
    full_path = vfs_get_fullpath(dir);
    full_path = (char *)realloc(full_path, strlen(full_path) + strlen(path) + 2);
    assert(full_path); // 这如果申请失败了，那么炸了算了
    if (full_path[strlen(full_path) - 1] != '/') strcat(full_path, "/");
    strcat(full_path, path);
    klogd("full path is %s", full_path);
  }

  describtor_t fd = descriptor_open(full_path);
  if (!fd) goto err;

  klogd("we get the fd %p", fd);
  free(full_path);

  int id = alloc_next_id(man);
  avltree_insert(man->avltree, id, fd);
  return id;

err:
  free(full_path);
  return -1;
}

// @return 0: success, -1: failed
int resman_close(resman_t man, i32 id) {
  describtor_t fd = avltree_get(man->avltree, id);
  if (!fd) return -1; // 无效的文件描述符

  int status = descriptor_close(fd);
  if (status == -1) return -1;

  avltree_delete(man->avltree, id);
  man->next_id = id;
  return 0;
}

static void clone_avltree(avltree_t src, avltree_t dst) {
  if (src == null) return;
  if (src->left) clone_avltree(src->left, dst);
  avltree_insert(dst, src->key, src->value);
  describtor_t fd = src->value;
  fd->refrence_cout++;
  if (src->right) clone_avltree(src->right, dst);
}

resman_t resman_clone(resman_t man) {
  resman_t new_resman = resman_alloc();
  resman_init(new_resman);
  clone_avltree(man->avltree, new_resman->avltree);
  return new_resman;
}
static void close_file(void *p) {
  descriptor_close(p);
}
void resman_deinit(resman_t man) {
  avltree_free_with(man->avltree, close_file);
}
