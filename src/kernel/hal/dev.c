// This code is released under the MIT License
#include <kernel.h>
int          devfs_id = 0;
rbtree_sp_t  dev_rbtree;
extern vdisk vdisk_ctl[26];
int          devfs_mount(cstr src, vfs_node_t node) {
  node->fsid = devfs_id;
  for (int i = 0; have_vdisk(i); i++) {
    vfs_child_append(node, vdisk_ctl[i].DriveName, null);
    rbtree_sp_insert(dev_rbtree, vdisk_ctl[i].DriveName, (void *)i);
  }
  return 0;
}
static int devfs_mkdir(void *parent, cstr name, vfs_node_t node) {
  klogw("You cannot create directory in devfs");
  node->fsid = 0; // 交给vfs处理
  return 0;
}
static void dummy() {}
// offset 必须能被扇区大小整除
static int  devfs_read(void *file, void *addr, size_t offset, size_t size) {
  int dev_id = (int)file;
  int sector_size;
  if (vdisk_ctl[dev_id].flag == 0) return -1;
  sector_size                     = vdisk_ctl[dev_id].sector_size;
  int   padding_up_to_sector_size = PADDING_UP(size, sector_size);
  void *buf;
  if (padding_up_to_sector_size == size) {
    buf = addr;
  } else {
    buf = page_malloc(padding_up_to_sector_size);
  }
  int sectors_to_do = size / sector_size;
  Disk_Read(offset / sector_size, sectors_to_do, buf, dev_id);
  if (padding_up_to_sector_size != size) {
    memcpy(addr, buf, size);
    page_free(buf, padding_up_to_sector_size);
  }
  return 0;
}
static int devfs_write(void *file, const void *addr, size_t offset, size_t size) {
  int dev_id = (int)file;
  int sector_size;
  if (vdisk_ctl[dev_id].flag == 0) return -1;
  sector_size                     = vdisk_ctl[dev_id].sector_size;
  int   padding_up_to_sector_size = PADDING_UP(size, sector_size);
  void *buf;
  if (padding_up_to_sector_size == size) {
    buf = (void *)addr;
  } else {
    buf = page_malloc(padding_up_to_sector_size);
    memset(buf, 0, padding_up_to_sector_size);
    memcpy(buf, addr, size);
  }
  int sectors_to_do = size / sector_size;
  Disk_Write(offset / sector_size, sectors_to_do, buf, dev_id);
  if (padding_up_to_sector_size != size) {
    memcpy(buf, addr, size);
    page_free(buf, padding_up_to_sector_size);
  }
  return 0;
}
static void devfs_open(void *parent, cstr name, vfs_node_t node) {
  node->handle = rbtree_sp_get(dev_rbtree, name);
  node->type   = file_block;
  node->size   = disk_Size((int)node->handle);
}
static struct vfs_callback callbacks = {
    .mount   = devfs_mount,
    .unmount = (void *)dummy,
    .mkdir   = devfs_mkdir,
    .mkfile  = (void *)dummy,
    .open    = devfs_open,
    .close   = (void *)dummy,
    .stat    = (void *)dummy,
    .read    = devfs_read,
    .write   = devfs_write,
};

void devfs_regist() {
  devfs_id = vfs_regist("devfs", &callbacks);
}

int dev_get_sector_size(char *path) {
  vfs_node_t node = vfs_open(path);
  if (node == null) return -1;
  if (node->fsid != devfs_id) return -1; //不是devfs
  int dev_id      = (int)node->handle;
  int sector_size = vdisk_ctl[dev_id].sector_size;
  return sector_size;
}
int dev_get_size(char *path) {
  vfs_node_t node = vfs_open(path);
  if (node == null) return -1;
  if (node->fsid != devfs_id) return -1; //不是devfs
  int dev_id = (int)node->handle;
  int size   = disk_Size(dev_id);
  return size;
}

// 1 : HDD
// 2 : cdrom
int dev_get_type(char *path) {
  vfs_node_t node = vfs_open(path);
  if (node == null) return -1;
  if (node->fsid != devfs_id) return -1; //不是devfs
  int dev_id = (int)node->handle;
  int type   = vdisk_ctl[dev_id].flag;
  return type;
}