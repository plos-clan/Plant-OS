#pragma once
#include <data-structure.h>
#include <libc-base.h>

typedef void *(*vfs_openfs_t)();

typedef void *(*vfs_open_t)(void *parent, cstr name);
typedef void (*vfs_close_t)(void *current);
typedef void (*vfs_resize_t)(void *current, u64 size);

// 读写一个文件
typedef int (*vfs_write_t)(void *file, const void *addr, size_t offset, size_t size);
typedef int (*vfs_read_t)(void *file, void *addr, size_t offset, size_t size);

typedef int (*vfs_create_dir_t)(void *file, cstr name);
typedef struct vfs_callback {
  vfs_open_t       opendir;
  vfs_open_t       openfile;
  vfs_close_t      closedir;
  vfs_close_t      closefile;
  vfs_read_t       read;
  vfs_write_t      write;
  vfs_create_dir_t mkdir;
  void (*free)(void *);
} *vfs_callback_t;

typedef struct vfs_nmsb *vfs_nmsb_t;
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
void register_fat();

#define PATH_MAX     4096
#define FILENAME_MAX 256
