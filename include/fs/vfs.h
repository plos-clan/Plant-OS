#pragma once
#include <data-structure.h>
#include <libc-base.h>
// * 所有时间请使用 GMT 时间 *

// 读写时请 padding 到 PAGE_SIZE 的整数倍
#define FILE_BLKSIZE PAGE_SIZE

typedef struct vfs_node *vfs_node_t;

typedef int (*vfs_mount_t)(cstr src, vfs_node_t node);
typedef void (*vfs_unmount_t)(void *root);

typedef void (*vfs_open_t)(void *parent, cstr name, vfs_node_t node);
typedef void (*vfs_close_t)(void *current);
typedef void (*vfs_resize_t)(void *current, u64 size);

// 读写一个文件
typedef int (*vfs_write_t)(void *file, const void *addr, size_t offset, size_t size);
typedef int (*vfs_read_t)(void *file, void *addr, size_t offset, size_t size);

typedef int (*vfs_stat_t)(void *file, vfs_node_t node);

// 创建一个文件或文件夹
typedef int (*vfs_mk_t)(void *parent, cstr name, vfs_node_t node);

// 映射文件从 offset 开始的 size 大小
typedef void *(*vfs_mapfile_t)(void *file, size_t offset, size_t size);

enum {
  file_none,   // 未获取信息
  file_dir,    // 文件夹
  file_block,  // 块设备，如硬盘
  file_stream, // 流式设备，如终端
};

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
} *vfs_callback_t;

struct vfs_node {
  vfs_node_t parent;      // 父目录
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
  vfs_node_t root;        // 根目录
};

struct fd {
  void  *file;
  size_t offset;
  bool   readable;
  bool   writeable;
};

extern struct vfs_callback vfs_empty_callback;

extern vfs_node_t rootdir;

vfs_node_t vfs_child_append(vfs_node_t parent, cstr name, void *handle);

bool vfs_init();
void vfs_deinit();

int vfs_mount(cstr src, vfs_node_t node);

int vfs_regist(cstr name, vfs_callback_t callback);

#define PATH_MAX     4096
#define FILENAME_MAX 256

void tmpfs_regist();
void fatfs_regist();
void iso9660_regist();

vfs_node_t vfs_open(cstr str);
int        vfs_mkdir(cstr name);

int  vfs_read(vfs_node_t file, void *addr, size_t offset, size_t size);
int  vfs_mkfile(cstr name);
int  vfs_write(vfs_node_t file, void *addr, size_t offset, size_t size);
int  vfs_unmount(cstr path);
int  vfs_close(vfs_node_t node);
void vfs_update(vfs_node_t node);