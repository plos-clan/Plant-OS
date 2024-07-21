#pragma once
#include "vfs.h"
#include <define.h>
#include <type.h>

void pfs_get_file_index_by_path(vfs_t *vfs, char *path, u32 start_block, u32 *err, u32 *idx,
                                u32 *dict_block);
void pfs_delete_name_link(vfs_t *vfs, u32 next, u32 dict_block);
void reg_pfs();
void pfs_flush_bitmap(vfs_t *vfs);

#pragma pack(1)

typedef struct {
  u8   jmp[2];
  u8   sign[4];           // "PFS\xff"
  u32  sec_bitmap_start;  // 位图起始扇区号
  u32  resd_sector_start; // 都为0就没有保留
  u32  resd_sector_end;
  u32  first_sector_of_bitmap;
  u32  root_dict_block;
  char volid[16]; // "POWERINTDOS\x00"
} pfs_mbr;
typedef struct {
  u8 r    : 1;
  u8 w    : 1;
  u8 e    : 1;
  u8 resd : 5;
} pfs_attr;
typedef struct {
  char type; // 0：未使用 1:文件 2.文件夹 3.长文件名（长文件名使用另外一个结构）
  char name[14]; // 第十四个字节如果是0xff
                 // 那么说明没读完，要继续往下读（通过next）
  u32 dat; // 如果是文件，那么就是data_block的编号，如果是文件夹，那么就是该文件夹的目录区的编号
  u32      time; // unix时间戳
  u32      size;
  pfs_attr attr; // 权限
  u32 next; // 下一个与此有关的inode在该目录区的第几项（文件名），诺此处为0，则表示没有，继续按顺序往下读
} pfs_inode; // 32字节
typedef struct {
  char type;     // it is always 3
  char name[27]; // 如果第26个字节是0xff 那么就继续往下读
  u32  next;     // 0 or other
} pfs_inode_of_long_file_name;
typedef struct {
  u8  data[508];
  u32 next;
} pfs_data_block; // 512b(one sec)
typedef struct {
  pfs_inode inodes[15]; // 480b
  u32       next;
  char      resd[28];
} pfs_dict_block; // 512b
#pragma pack()
typedef struct pfs pfs_t;
typedef struct pfs {
  List *file_list;
  List *bitmap;
  List *bitmap_buffer;
  u32   resd_sec_start;
  u32   resd_sec_end;
  u32   first_sec_of_bitmap;
  u32   sec_bitmap_start;
  u32   root_dict_block;
  void (*read_block)(pfs_t *pfs, u32 lba, u32 numbers, void *buff);
  void (*write_block)(pfs_t *pfs, u32 lba, u32 numbers, void *buff);
  u8      disk_number;
  u32     current_dict_block;
  int64_t current_bitmap_block;
  u8     *bitmap_buff;
  List   *prev_dict_block;
} pfs_t;
typedef struct {
  char    *name;
  u32      size;
  u32      block;
  u32      time;
  pfs_attr attr;
} pfs_file_list;
#define total_bits_of_one_sec     ((512 - 4) * 8)
#define used(bitmap, index)       bitmap[index / 8] |= (1 << (index % 8))
#define unused(bitmap, index)     bitmap[index / 8] &= ~(1 << (index % 8))
#define bit_get(bitmap, index)    (bitmap[index / 8] & (1 << (index % 8)))
#define set_next(bitmap, next)    *((u32 *)((uintptr_t)bitmap + 508)) = (next)
#define get_next(bitmap)          (*((u32 *)((uintptr_t)bitmap + 508)))
#define block2sector(block, _pfs) ((block) + ((_pfs)->first_sec_of_bitmap))
u32 pfs_create_inode(vfs_t *vfs, u32 dict_block);
// void pfs_format(pfs_t p, char* volid);
// void init_pfs(pfs_t p);
// u32 pfs_create_inode(u32 dict_block);
// void pfs_ls(u32 dict_block);
// u32 pfs_get_filesize(char* filename, u32 dict_block, u32* err);
// void pfs_read_file(char* filename, void* buff, u32 dict_block);
// void pfs_create_file(char* filename, u32 dict_block);
// void pfs_create_dict(char* name, u32 dict_block);
// u32 pfs_get_dict_block_by_name(char* name,
//                                     u32 dict_block,
//                                     u32* err);
// u32 pfs_get_idx_of_inode_by_name(char* name,
//                                       u32 dict_block,
//                                       u32* err);
// void pfs_write_file(char* filename,
//                     u32 size,
//                     void* buff,
//                     u32 dict_block);
// u32 pfs_get_dict_block_by_path(char* path,
//                                     char** end,
//                                     u32 start_block,
//                                     u32* err);
// void pfs_get_file_index_by_path(char* path,
//                                 u32 start_block,
//                                 u32* err,
//                                 u32* idx,
//                                 u32* dict_block);

#pragma pack()
