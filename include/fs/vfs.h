#pragma once
#include <data-structure.h>
#include <libc-base.h>

typedef void *(*vfs_openfs_t)();

typedef void *(*vfs_open_t)(void *parent, cstr name);
typedef void (*vfs_close_t)(void *current);
typedef void (*vfs_resize_t)(void *current, u64 size);

typedef struct vfs_callback {
  vfs_open_t  opendir;
  vfs_open_t  openfile;
  vfs_close_t closedir;
  vfs_close_t closefile;
} *vfs_callback_t;

typedef struct vfs *vfs_t;
struct vfs {
  vfs_t    parent;      // 父目录
  cstr     name;        // 名称
  u64      realsize;    // 项目真实占用的空间 (可选)
  u64      size;        // 文件大小或文件夹内项目数
  u64      createtime;  // 创建时间
  u64      readtime;    // 最后读取时间
  u64      writetime;   // 最后写入时间
  u32      owner;       // 所有者
  u32      group;       // 所有组
  u32      permissions; // 权限
  rbtree_t child;
};

extern vfs_t rootdir;

bool vfs_init();
void vfs_deinit();
void register_fat();