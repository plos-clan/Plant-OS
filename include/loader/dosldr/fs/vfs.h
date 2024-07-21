#pragma once
#include <define.h>
#include <dosldr/list.h>
#include <dosldr/tsk.h>
#include <type.h>
typedef enum {
  FLE,
  DIR,
  RDO,
  HID,
  SYS
} ftype;
typedef struct {
  char  name[255];
  ftype type;
  u32   size;
  u16   year, month, day;
  u16   hour, minute;
} vfs_file;

typedef bool (*vfs_read_t)(struct vfs_t *vfs, char *path, char *buffer);
typedef bool (*vfs_write_t)(struct vfs_t *vfs, char *path, char *buffer, int size);
typedef bool (*vfs_del_t)(struct vfs_t *vfs, char *path);
typedef bool (*vfs_create_t)(struct vfs_t *vfs, char *filename);
typedef bool (*vfs_raname_t)(struct vfs_t *vfs, char *filename, char *filename_of_new);
typedef bool (*vfs_cd_t)(struct vfs_t *vfs, char *dictName);
typedef int (*vfs_filesize_t)(struct vfs_t *vfs, char *filename);
typedef void (*vfs_copy_cache_t)(struct vfs_t *dest, struct vfs_t *src);

typedef struct vfs_t {
  struct List *path;
  void        *cache;
  char         FSName[255];
  int          disk_number;
  u8           drive; // 大写（必须）
  vfs_file *(*FileInfo)(struct vfs_t *vfs, char *filename);
  struct List *(*ListFile)(struct vfs_t *vfs, char *dictpath);
  vfs_read_t   ReadFile;
  vfs_write_t  WriteFile;
  vfs_del_t    DelFile;
  vfs_del_t    DelDict;
  vfs_create_t CreateFile;
  vfs_create_t CreateDict;
  vfs_raname_t RenameFile;
  bool (*Attrib)(struct vfs_t *vfs, char *filename, ftype type);
  bool (*Format)(u8 disk_number);
  void (*InitFs)(struct vfs_t *vfs, u8 disk_number);
  void (*DeleteFs)(struct vfs_t *vfs);
  bool (*Check)(u8 disk_number);
  vfs_cd_t         cd;
  vfs_filesize_t   FileSize;
  vfs_copy_cache_t CopyCache;
  int              flag;
} vfs_t;

List     *vfs_listfile(char *dictpath);
bool      vfs_readfile(char *path, char *buffer);
bool      vfs_writefile(char *path, char *buffer, int size);
u32       vfs_filesize(char *filename);
bool      vfs_register_fs(vfs_t vfs);
bool      vfs_renamefile(char *filename, char *filename_of_new);
bool      vfs_change_path(char *dictName);
bool      vfs_deldir(char *dictname);
bool      vfs_delfile(char *filename);
bool      vfs_createfile(char *filename);
bool      vfs_createdict(char *filename);
void      vfs_getPath(char *buffer);
bool      vfs_change_disk(u8 drive);
bool      vfs_mount_disk(u8 disk_number, u8 drive);
u32       vfs_filesize(char *filename);
bool      vfs_readfile(char *path, char *buffer);
bool      vfs_change_disk_for_task(u8 drive, struct TASK *task);
bool      vfs_format(u8 disk_number, char *FSName);
bool      vfs_check_mount(u8 drive);
bool      vfs_unmount_disk(u8 drive);
bool      vfs_attrib(char *filename, ftype type);
vfs_file *vfs_fileinfo(char *filename);
void      init_vfs();
