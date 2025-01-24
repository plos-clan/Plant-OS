// This code is released under the MIT License

#include <kernel.h>

typedef struct resman {
  avltree_t avltree;
  i32       next_id;
} *resman_t;

typedef struct descriptor {
  vfs_node_t node;
  i32        refrence_cout;
} *describtor_t;

st_t descriptors;
void resman_init(resman_t man);

static i32 alloc_next_id(resman_t man) {
  while (avltree_get(man->avltree, man->next_id++))
    ;
  return man->next_id - 1;
}

void resman_descriptors_init() {
  descriptors = st_new();
}

// **NOTE**: the path is a absolute path, not a relative path
static describtor_t descriptor_open(char *path) {
  // 首先，我们要看看能不能命中符号表
  describtor_t fd = st_get(descriptors, path);
  if(fd) {
    fd->refrence_cout++;
    return fd;
  }
  // 如果没有命中，那么我们就要打开文件
  vfs_node_t node = vfs_open(path);
  if (!node) return null; // 打开失败
  // 创建文件描述符
  fd = malloc(sizeof(struct descriptor));
  fd->node = node;
  fd->refrence_cout = 1;
  st_insert(descriptors, path, fd);
  return fd;
}
// @return: -1 works failed, 0 works successfully
static int descriptor_close(char *path) {
  describtor_t fd = st_get(descriptors, path);
  if (!fd) return -1;
  if (--fd->refrence_cout == 0) {
    vfs_close(fd->node);
    st_remove(descriptors, path);
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
  describtor_t fd_dir = avltree_get(man->avltree, working_dir);
  if (!fd_dir) return -1; // 无效的工作目录
  vfs_node_t dir = fd_dir->node;
  if (dir->type != file_dir) return -1; // 工作目录不是一个目录
  char *full_path;
  if (path[0] == '/') {
    dir = vfs_open("/");
    assert(dir, "open / failed");
    full_path = strdup(path);
  } else {
    // full_path = malloc(strlen(dir->name) + strlen(path) + 2);
    // sprintf(full_path, "%s/%s", dir->name, path);
  }

}
void resman_deinit(resman_t man) {
  avltree_free(man->avltree);
}
