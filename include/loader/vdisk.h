#pragma once
#include <define.h>

typedef struct {
  void (*read)(int drive, u8 *buffer, u32 number, u32 lba);
  void (*write)(int drive, u8 *buffer, u32 number, u32 lba);
  int  flag;
  u32  size; // 大小
  char drive_name[50];
} vdisk;

int  vdisk_init();
int  regist_vdisk(vdisk vd);
int  logout_vdisk(int drive);
int  rw_vdisk(int drive, u32 lba, u8 *buffer, u32 number, int read);
bool have_vdisk(int drive);
void vdisk_read(u32 lba, u32 number, void *buffer, int drive);
void vdisk_write(u32 lba, u32 number, void *buffer, int drive);
