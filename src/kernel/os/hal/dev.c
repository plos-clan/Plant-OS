// This code is released under the MIT License
#include <kernel.h>

rbtree_sp_t dev_rbtree;

int devfs_mount(cstr src, vfs_node_t node) {
  for (int i = 0; have_vdisk(i); i++) {
    char buf[100];
    sprintf(buf, "dev%d", i);
    vfs_child_append(node, buf, null);
    rbtree_sp_insert(dev_rbtree, buf, (void *)i);
  }
  return 0;
}

static void dummy() {}

static struct vfs_callback callbacks = {
    .mount   = devfs_mount,
    .unmount = (void *)dummy,
    .mkdir   = (void *)dummy,
    .mkfile  = (void *)dummy,
    .open    = (void *)dummy,
    .close   = (void *)dummy,
    .stat    = (void *)dummy,
    .read    = (void *)dummy,
    .write   = (void *)dummy,
};

void devfs_regist() {
  vfs_regist("devfs", &callbacks);
}
