#pragma once
#include <data-structure.h>
#include <libc-base.h>

// * 所有时间请使用 GMT 时间 *

// 读写时请 padding 到 PAGE_SIZE 的整数倍
#define FILE_BLKSIZE PAGE_SIZE

typedef struct vfs_node *vfs_node_t;

typedef int (*vfs_mount_t)(cstr src, vfs_node_t node);
typedef void (*vfs_unmount_t)(void *root);

/**
 *\brief 打开一个文件
 *
 *\param parent   父目录句柄
 *\param name     文件名
 *\param node     文件节点
 */
typedef void (*vfs_open_t)(void *parent, cstr name, vfs_node_t node);

/**
 *\brief 关闭一个文件
 *
 *\param current  当前文件句柄
 */
typedef void (*vfs_close_t)(void *current);

/**
 *\brief 重设文件大小
 *
 *\param current  当前文件句柄
 *\param size     新的大小
 */
typedef void (*vfs_resize_t)(void *current, u64 size);

/**
 *\brief 写入一个文件
 *
 *\param file     文件句柄
 *\param addr     写入的数据
 *\param offset   写入的偏移
 *\param size     写入的大小
 */
typedef ssize_t (*vfs_write_t)(void *file, const void *addr, size_t offset, size_t size) __nnull(2)
    __attr_readonly(2, 4);

/**
 *\brief 读取一个文件
 *
 *\param file     文件句柄
 *\param addr     读取的数据
 *\param offset   读取的偏移
 *\param size     读取的大小
 */
typedef ssize_t (*vfs_read_t)(void *file, void *addr, size_t offset, size_t size) __nnull(2)
    __attr_writeonly(2, 4);

/**
 *\brief 获取文件信息
 *
 *\param file     文件句柄
 *\param node     文件节点
 */
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
  vfs_stat_t    stat;
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
  u16        type;        // 类型
  u16        fsid;        // 文件系统的 id
  void      *handle;      // 操作文件的句柄
  list_t     child;       // 子目录和子文件
  vfs_node_t root;        // 根目录
};

struct fd {
  void  *file;
  size_t offset;
  bool   readable;
  bool   writeable;
};

extern vfs_node_t rootdir; // vfs 根目录

vfs_node_t vfs_child_append(vfs_node_t parent, cstr name, void *handle);

bool vfs_init();

/**
 *\brief 注册一个文件系统
 *
 *\param name      文件系统名称
 *\param callback  文件系统回调
 *\return 文件系统 id
 */
int vfs_regist(cstr name, vfs_callback_t callback);

#define PATH_MAX     4096 // 路径最大长度
#define FILENAME_MAX 256  // 文件名最大长度

vfs_node_t vfs_open(cstr str);

/**
 *\brief 创建文件夹
 *
 *\param name     文件夹名
 *\return 0 成功，-1 失败
 */
int vfs_mkdir(cstr name);
/**
 *\brief 创建文件
 *
 *\param name     文件名
 *\return 0 成功，-1 失败
 */
int vfs_mkfile(cstr name);

/**
 *\brief 读取文件
 *
 *\param file     文件句柄
 *\param addr     读取的数据
 *\param offset   读取的偏移
 *\param size     读取的大小
 *\return 0 成功，-1 失败
 */
ssize_t vfs_read(vfs_node_t file, void *addr, size_t offset, size_t size) __nnull(1, 2)
    __attr_writeonly(2, 4);
/**
 *\brief 写入文件
 *
 *\param file     文件句柄
 *\param addr     写入的数据
 *\param offset   写入的偏移
 *\param size     写入的大小
 *\return 0 成功，-1 失败
 */
ssize_t vfs_write(vfs_node_t file, const void *addr, size_t offset, size_t size) __nnull(1, 2)
    __attr_readonly(2, 4);

/**
 *\brief 挂载文件系统
 *
 *\param src      源文件地址
 *\param node     挂载到的节点
 *\return 0 成功，-1 失败
 */
int vfs_mount(cstr src, vfs_node_t node);
/**
 *\brief 卸载文件系统
 *
 *\param path     文件路径
 *\return 0 成功，-1 失败
 */
int vfs_unmount(cstr path);

/**
 *\brief 关闭文件
 *
 *\param node     文件节点
 *\return 0 成功，-1 失败
 */
int vfs_close(vfs_node_t node);

/**
 *\brief 更新文件信息
 *
 *\param node     文件节点
 */
void vfs_update(vfs_node_t node);
