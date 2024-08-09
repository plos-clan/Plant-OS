#pragma once
#include <data-structure.h>
#include <kernel/mtask.h>
#include <libc-base.h>

typedef enum {
  vfs_FLE,
  vfs_DIR,
  vfs_RDO,
  vfs_HID,
  vfs_SYS
} ftype;

typedef struct {
  char  name[255];
  ftype type;
  u32   size;
  u16   year, month, day;
  u16   hour, minute;
} vfs_file;

typedef struct vfs_t {
  list_t path;
  void  *cache;
  char   FSName[255];
  int    disk_number;
  u8     drive; // 大写（必须）
  vfs_file *(*FileInfo)(struct vfs_t *vfs, char *filename);
  list_t (*ListFile)(struct vfs_t *vfs, char *dictpath);
  bool (*ReadFile)(struct vfs_t *vfs, char *path, char *buffer);
  bool (*WriteFile)(struct vfs_t *vfs, char *path, char *buffer, int size);
  bool (*DelFile)(struct vfs_t *vfs, char *path);
  bool (*DelDict)(struct vfs_t *vfs, char *path);
  bool (*CreateFile)(struct vfs_t *vfs, char *filename);
  bool (*CreateDict)(struct vfs_t *vfs, char *filename);
  bool (*RenameFile)(struct vfs_t *vfs, char *filename, char *filename_of_new);
  bool (*Attrib)(struct vfs_t *vfs, char *filename, ftype type);
  bool (*Format)(u8 disk_number);
  void (*InitFs)(struct vfs_t *vfs, u8 disk_number);
  void (*DeleteFs)(struct vfs_t *vfs);
  bool (*Check)(u8 disk_number);
  bool (*cd)(struct vfs_t *vfs, char *dictName);
  int (*FileSize)(struct vfs_t *vfs, char *filename);
  void (*CopyCache)(struct vfs_t *dest, struct vfs_t *src);
  int flag;
} vfs_t;

bool   vfs_change_disk_for_task(u8 drive, mtask *task);
void   init_vfs();
bool   vfs_readfile(char *path, char *buffer);
bool   vfs_register_fs(vfs_t vfs);
bool   vfs_mount_disk(u8 disk_number, u8 drive);
bool   vfs_change_disk(u8 drive);
list_t vfs_listfile(char *dictpath);
void   register_fat();
u32    vfs_filesize(char *filename);