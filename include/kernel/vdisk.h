#pragma once
#include <define.h>

typedef struct {
  void (*Read)(char drive, u8 *buffer, u32 number, u32 lba);
  void (*Write)(char drive, u8 *buffer, u32 number, u32 lba);
  int  flag;
  u32  size; // 大小
  char DriveName[50];
} vdisk;

int  init_vdisk();
int  register_vdisk(vdisk vd);
int  logout_vdisk(char drive);
void Disk_Read(u32 lba, u32 number, void *buffer, char drive);
u32  disk_Size(char drive);
bool DiskReady(char drive);
void Disk_Write(u32 lba, u32 number, const void *buffer, char drive);
bool CDROM_Read(u32 lba, u32 number, void *buffer, char drive);
bool have_vdisk(char drive);