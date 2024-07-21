#pragma once
#include "vfs.h"
#include <define.h>
#include <type.h>

struct FAT_CACHE {
  u32                  ADR_DISKIMG;
  struct FAT_FILEINFO *root_directory;
  struct LIST         *directory_list;
  struct LIST         *directory_max_list;
  struct LIST         *directory_clustno_list;
  int                 *fat;
  int                  FatMaxTerms;
  u32                  ClustnoBytes;
  u16                  RootMaxFiles;
  u32                  RootDictAddress;
  u32                  FileDataAddress;
  u32                  imgTotalSize;
  u16                  SectorBytes;
  u32                  Fat1Address;
  u32                  Fat2Address;
  u8                  *FatClustnoFlags;
  int                  type;
};

typedef struct {
  struct FAT_CACHE     dm;
  struct FAT_FILEINFO *dir;
} fat_cache;

struct FAT_FILEINFO {
  u8   name[8], ext[3], type;
  char reserve;
  u8   create_time_tenth;
  u16  create_time, create_date, access_date, clustno_high;
  u16  update_time, update_date, clustno_low;
  u32  size;
};

#define get_dm(vfs)            (((fat_cache *)(vfs->cache))->dm)
#define get_now_dir(vfs)       (((fat_cache *)(vfs->cache))->dir)
#define get_clustno(high, low) ((high << 16) | (low & 0xffff))
#define clustno_end(type)      (0xfffffff & ((((1 << (type - 1)) - 1) << 1) + 1))

#define BS_jmpBoot       0
#define BS_OEMName       3
#define BPB_BytsPerSec   11
#define BPB_SecPerClus   13
#define BPB_RsvdSecCnt   14
#define BPB_NumFATs      16
#define BPB_RootEntCnt   17
#define BPB_TotSec16     19
#define BPB_Media        21
#define BPB_FATSz16      22
#define BPB_SecPerTrk    24
#define BPB_NumHeads     26
#define BPB_HiddSec      28
#define BPB_TotSec32     32
#define BPB_FATSz32      36
#define BPB_ExtFlags     40
#define BPB_FSVer        42
#define BPB_RootClus     44
#define BPB_FSInfo       48
#define BPB_BkBootSec    50
#define BPB_Reserved     52
#define BPB_Fat32ExtByts 28
#define BS_DrvNum        36
#define BS_Reserved1     37
#define BS_BootSig       38
#define BS_VolD          39
#define BS_VolLab        43
#define BS_FileSysType   54
void file_savefat(int *fat, int clustno, int length, vfs_t *vfs);
void mkfile(char *name, vfs_t *vfs);
void Register_fat_fileSys();
