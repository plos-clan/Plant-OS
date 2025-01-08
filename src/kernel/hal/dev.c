// This code is released under the MIT License
#include <kernel.h>

static int         devfs_id = 0;
static rbtree_sp_t dev_rbtree;
static vfs_node_t  devfs_root = null;

extern vdisk vdisk_ctl[26];

int devfs_mount(cstr src, vfs_node_t node) {
  if (src != null) return -1;
  if (devfs_root) {
    error("devfs has been mounted");
    return -1;
  }
  node->fsid = devfs_id;
  devfs_root = node;
  return 0;
}

void devfs_regist_dev(int drive_id) {
  if (have_vdisk(drive_id)) {
    vfs_child_append(devfs_root, vdisk_ctl[drive_id].drive_name, null);
    rbtree_sp_insert(dev_rbtree, vdisk_ctl[drive_id].drive_name, (void *)drive_id);
  }
}

static int devfs_mkdir(void *parent, cstr name, vfs_node_t node) {
  klogw("You cannot create directory in devfs");
  node->fsid = 0; // 交给vfs处理
  return 0;
}
static void    dummy() {}
// offset 必须能被扇区大小整除
static ssize_t devfs_read(void *file, void *addr, size_t offset, size_t size) {
  int    dev_id = (int)file;
  size_t sector_size;
  size_t sectors_to_do;
  if (vdisk_ctl[dev_id].flag == 0) return -1;
  sector_size                      = vdisk_ctl[dev_id].sector_size;
  size_t padding_up_to_sector_size = PADDING_UP(size, sector_size);
  offset                           = PADDING_UP(offset, sector_size);
  void *buf;
  if (vdisk_ctl[dev_id].type == VDISK_STREAM) goto read;
  if (offset > vdisk_ctl[dev_id].size) return 0;
  if (vdisk_ctl[dev_id].size < offset + padding_up_to_sector_size) {
    // 计算需要读取的扇区数
    padding_up_to_sector_size = vdisk_ctl[dev_id].size - offset;
    if (size > padding_up_to_sector_size) { size = padding_up_to_sector_size; }
  }
read:
  sectors_to_do = padding_up_to_sector_size / sector_size;
  if (padding_up_to_sector_size == size) {
    buf = addr;
  } else {
    buf = page_alloc(padding_up_to_sector_size);
  }
  vdisk_read(offset / sector_size, sectors_to_do, buf, dev_id);
  if (padding_up_to_sector_size != size) {
    memcpy(addr, buf, size);
    page_free(buf, padding_up_to_sector_size);
  }
  return size;
}

static ssize_t devfs_write(void *file, const void *addr, size_t offset, size_t size) {
  int    dev_id = (int)file;
  size_t sector_size;
  size_t sectors_to_do;
  if (vdisk_ctl[dev_id].flag == 0) return -1;
  sector_size                      = vdisk_ctl[dev_id].sector_size;
  size_t padding_up_to_sector_size = PADDING_UP(size, sector_size);
  offset                           = PADDING_UP(offset, sector_size);

  void *buf;
  if (vdisk_ctl[dev_id].type == VDISK_STREAM) goto write;
  if (offset > vdisk_ctl[dev_id].size) return 0;
  if (vdisk_ctl[dev_id].size < offset + padding_up_to_sector_size) {
    padding_up_to_sector_size = vdisk_ctl[dev_id].size - offset;
    if (size > padding_up_to_sector_size) { size = padding_up_to_sector_size; }
  }
write:
  sectors_to_do = padding_up_to_sector_size / sector_size;

  if (padding_up_to_sector_size == size) {
    buf = (void *)addr;
  } else {
    buf = page_alloc(padding_up_to_sector_size);
    vdisk_read(offset / sector_size, sectors_to_do, buf, dev_id);
    memcpy(buf, addr, size);
  }
  vdisk_write(offset / sector_size, sectors_to_do, buf, dev_id);
  if (padding_up_to_sector_size != size) { page_free(buf, padding_up_to_sector_size); }
  return size;
}

int devfs_stat(void *handle, vfs_node_t node) {
  if (node->type == file_dir) return 0;
  node->handle = rbtree_sp_get(dev_rbtree, node->name);
  node->type   = vdisk_ctl[(int)node->handle].type == VDISK_STREAM ? file_stream : file_block;
  node->size   = disk_size((int)node->handle);
  return 0;
}

static void devfs_open(void *parent, cstr name, vfs_node_t node) {
  if (node->type == file_dir) return;
  node->handle = rbtree_sp_get(dev_rbtree, name);
  node->type   = vdisk_ctl[(int)node->handle].type == VDISK_STREAM ? file_stream : file_block;
  node->size   = disk_size((int)node->handle);
}

static struct vfs_callback callbacks = {
    .mount   = devfs_mount,
    .unmount = (void *)dummy,
    .mkdir   = devfs_mkdir,
    .mkfile  = (void *)dummy,
    .open    = devfs_open,
    .close   = (void *)dummy,
    .stat    = devfs_stat,
    .read    = devfs_read,
    .write   = devfs_write,
};

void devfs_regist() {
  devfs_id = vfs_regist("devfs", &callbacks);
}

int dev_get_sector_size(cstr path) {
  vfs_node_t node = vfs_open(path);
  if (node == null) return -1;
  if (node->fsid != devfs_id) return -1; //不是devfs
  int dev_id      = (int)node->handle;
  int sector_size = vdisk_ctl[dev_id].sector_size;
  return sector_size;
}

int dev_get_size(cstr path) {
  vfs_node_t node = vfs_open(path);
  if (node == null) return -1;
  if (node->fsid != devfs_id) return -1; //不是devfs
  int dev_id = (int)node->handle;
  int size   = disk_size(dev_id);
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