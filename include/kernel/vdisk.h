#pragma once
#include <define.h>

typedef struct {
  void (*Read)(int drive, u8 *buffer, u32 number, u32 lba);
  void (*Write)(int drive, u8 *buffer, u32 number, u32 lba);
  int  flag;
  u32  size;        // 大小
  u32  sector_size; // 扇区大小
  char DriveName[50];
} vdisk;

int  vdisk_init();
int  register_vdisk(vdisk vd);
int  logout_vdisk(int drive);
void Disk_Read(u32 lba, u32 number, void *buffer, int drive);
u32  disk_Size(int drive);
bool DiskReady(int drive);
void Disk_Write(u32 lba, u32 number, const void *buffer, int drive);
bool CDROM_Read(u32 lba, u32 number, void *buffer, int drive);
bool have_vdisk(int drive);
