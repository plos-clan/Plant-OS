// This code is released under the MIT License
#include <kernel.h>
rbtree_sp_t dev_rbtree;
// typedef struct file {
//   u64 size;
//   void **blocks;
// } * file_t;

// static file_t file_alloc(size_t size) {
//   file_t file = malloc(sizeof(struct file));
//   if (file == null)
//     return null;
//   size_t mallocsize = PADDING_UP(size, FILE_BLKSIZE) * sizeof(void *);
//   file->size = size;
//   file->blocks = malloc(mallocsize);
//   memcpy(file->blocks, 0, mallocsize);
//   return file;
// }

// static void file_free(file_t file) {
//   if (file == null)
//     return;
//   for (size_t i = 0; i < PADDING_UP(file->size, FILE_BLKSIZE); i++) {
//     if (file->blocks[i])
//       free(file->blocks[i]);
//   }
//   free(file->blocks);
//   free(file);
// }

// int tmpfs_mkdir(void *parent, cstr name, vfs_node_t node) {
//   node->handle = null;
//   return 0;
// }

// int tmpfs_mkfile(void *parent, cstr name, vfs_node_t node) {
//   file_t file = file_alloc(node->size);
//   if (file == null)
//     return -1;
//   node->handle = file;
//   return 0;
// }

// int tmpfs_readfile(file_t file, void *addr, size_t offset, size_t size) {
//   if (file == null || addr == null)
//     return -1;

//   // size_t start_block = offset / FILE_BLKSIZE;
//   // size_t end_block   = (offset + size - 1) / FILE_BLKSIZE;

//   // for (size_t i = start_block; i <= end_block; i++) {
//   //   if (file->blocks[i] == null) {
//   //     memset(addr, 0, FILE_BLKSIZE);
//   //   } else {
//   //     memcpy(addr, file->blocks[i], FILE_BLKSIZE);
//   //   }
//   //   addr += FILE_BLKSIZE;
//   // }

//   return 0;
// }

// int tmpfs_writefile(file_t file, const void *addr, size_t offset, size_t size) {
//   return 0;
// }

// void tmpfs_open(void *parent, cstr name, vfs_node_t node) {
//   //
// }

// void tmpfs_close(file_t handle) { file_free(handle); }

int devfs_mount(cstr src, vfs_node_t node) {
  for (int i = 0;have_vdisk(i);i++) {
    char buf[100];
    sprintf(buf,"dev%d",i);
    vfs_child_append(node, buf, null);
    rbtree_sp_insert(dev_rbtree, buf, (void *)i);
  }
  return 0;
}

// void tmpfs_unmount(void *root) {
//   //
// }

// int tmpfs_stat(void *file, vfs_node_t node) { return 0; }
static void dummy() {}
static struct vfs_callback callbacks = {
    .mount = devfs_mount,
    .unmount = (void *)dummy,
    .mkdir = (void *)dummy,
    .mkfile = (void *)dummy,
    .open = (void *)dummy,
    .close = (void *)dummy,
    .stat = (void *)dummy,
    .read = (void *)dummy,
    .write = (void *)dummy,
};

void devfs_regist() { vfs_regist("devfs", &callbacks); }
