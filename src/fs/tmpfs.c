// This code is released under the MIT License

#include <fs.h>

typedef struct file {
  u64    size;
  void **blocks;
} *file_t;

static file_t file_alloc(size_t size) {
  file_t file = malloc(sizeof(struct file));
  if (file == null) return null;
  size_t mallocsize = PADDING_UP(size, FILE_BLKSIZE) * sizeof(void *);
  file->size        = size;
  file->blocks      = malloc(mallocsize);
  memset(file->blocks, 0, mallocsize);
  return file;
}

static void file_free(file_t file) {
  if (file == null) return;
  for (size_t i = 0; i < PADDING_UP(file->size, FILE_BLKSIZE); i++) {
    if (file->blocks[i]) free(file->blocks[i]);
  }
  free(file->blocks);
  free(file);
}

int tmpfs_mkdir(void *parent, cstr name, vfs_node_t node) {
  node->handle = null;
  return 0;
}

int tmpfs_mkfile(void *parent, cstr name, vfs_node_t node) {
  file_t file = file_alloc(node->size);
  if (file == null) return -1;
  node->handle = file;
  return 0;
}

size_t tmpfs_readfile(file_t file, void *addr, size_t offset, size_t size) {
  if (file == null || addr == null) return -1;

  // size_t start_block = offset / FILE_BLKSIZE;
  // size_t end_block   = (offset + size - 1) / FILE_BLKSIZE;

  // for (size_t i = start_block; i <= end_block; i++) {
  //   if (file->blocks[i] == null) {
  //     memset(addr, 0, FILE_BLKSIZE);
  //   } else {
  //     memcpy(addr, file->blocks[i], FILE_BLKSIZE);
  //   }
  //   addr += FILE_BLKSIZE;
  // }

  return 0;
}

int tmpfs_writefile(file_t file, const void *addr, size_t offset, size_t size) {
  return 0;
}

void tmpfs_open(void *parent, cstr name, vfs_node_t node) {
  //
}

void tmpfs_close(file_t handle) {
  file_free(handle);
}

int tmpfs_mount(cstr src, vfs_node_t node) {
  return -1;
}

void tmpfs_unmount(void *root) {
  //
}

int tmpfs_stat(void *file, vfs_node_t node) {
  return 0;
}

static struct vfs_callback callbacks = {
    .mount   = tmpfs_mount,
    .unmount = tmpfs_unmount,
    .open    = tmpfs_open,
    .close   = (vfs_close_t)tmpfs_close,
    .read    = (vfs_read_t)tmpfs_readfile,
    .write   = (vfs_write_t)tmpfs_writefile,
    .mkdir   = tmpfs_mkdir,
    .mkfile  = tmpfs_mkfile,
    .stat    = tmpfs_stat,
};

void tmpfs_regist() {
  vfs_regist("tmpfs", &callbacks);
}
