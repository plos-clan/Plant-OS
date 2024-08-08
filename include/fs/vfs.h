#pragma once
#include <data-structure.h>
#include <libc-base.h>

// 读写时请 padding 到 PAGE_SIZE 的整数倍
#define FILE_BLKSIZE PAGE_SIZE

#define PADDING_DOWN(size, to) ((size_t)(size) / (size_t)(to) * (size_t)(to))
#define PADDING_UP(size, to)   PADDING_DOWN((size_t)(size) + (size_t)(to) - (size_t)1, to)

typedef struct vfs_nmsb *vfs_nmsb_t;

typedef void *(*vfs_mount_t)(cstr src);
typedef void (*vfs_unmount_t)(void *root);

typedef void *(*vfs_open_t)(void *parent, cstr name, vfs_nmsb_t node);
typedef void (*vfs_close_t)(void *current);
typedef void (*vfs_resize_t)(void *current, u64 size);

// 读写一个文件
typedef int (*vfs_write_t)(void *file, const void *addr, size_t offset, size_t size);
typedef int (*vfs_read_t)(void *file, void *addr, size_t offset, size_t size);

typedef int (*vfs_stat_t)(void *file, vfs_nmsb_t node);

typedef int (*vfs_mk_t)(void *parent, cstr name, vfs_nmsb_t node);

typedef struct vfs_callback {
  vfs_mount_t   mount;
  vfs_unmount_t unmount;
  vfs_open_t    open;
  vfs_close_t   close;
  vfs_read_t    read;
  vfs_write_t   write;
  vfs_mk_t      mkdir;
  vfs_mk_t      mkfile;
  vfs_stat_t    stat; // 保留备用
  void (*free)(void *);
} *vfs_callback_t;

struct vfs_nmsb {
  vfs_nmsb_t parent;      // 父目录
  char      *name;        // 名称
  u64        realsize;    // 项目真实占用的空间 (可选)
  u64        size;        // 文件大小或若是文件夹则填0
  u64        createtime;  // 创建时间
  u64        readtime;    // 最后读取时间
  u64        writetime;   // 最后写入时间
  u32        owner;       // 所有者
  u32        group;       // 所有组
  u32        permissions; // 权限
  u8         type;        // 类型
  u16        fsid;        // 文件系统的 id
  void      *handle;      // 操作文件的句柄
  list_t     child;       //
};

struct {
  void  *file;
  size_t offset;
  bool   readable;
  bool   writeable;
};
// getcwd
extern vfs_nmsb_t rootdir;

bool vfs_init();
void vfs_deinit();

void vfs_regist(cstr name, vfs_callback_t callback);

#define PATH_MAX     4096
#define FILENAME_MAX 256
