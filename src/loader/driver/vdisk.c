#include <dosldr.h>
int   getReadyDisk(); // init.c
vdisk vdisk_ctl[10];
int   init_vdisk() {
  for (int i = 0; i < 10; i++) {
    vdisk_ctl[i].flag = 0; // 设置为未使用
  }
}
int register_vdisk(vdisk vd) {
  for (int i = 0; i < 10; i++) {
    if (!vdisk_ctl[i].flag) {
      vdisk_ctl[i] = vd; // 找到了！
      return i + ('A');  // 注册成功，返回drive
    }
  }
  printf("[vdisk]not found\n");
  return 0; // 注册失败
}
int logout_vdisk(char drive) {
  int indx = drive - ('A');
  if (indx > 10) {
    return 0; // 失败
  }
  if (vdisk_ctl[indx].flag) {
    vdisk_ctl[indx].flag = 0; // 设置为没有
    return 1;                 // 成功
  } else {
    return 0; // 失败
  }
}
int rw_vdisk(char drive, u32 lba, u8 *buffer, u32 number, int read) {
  int indx = drive - ('A');
  if (indx > 10) {
    return 0; // 失败
  }
  if (vdisk_ctl[indx].flag) {
    if (read) {
      vdisk_ctl[indx].Read(drive, buffer, number, lba);
      //for(;;);
    } else {
      vdisk_ctl[indx].Write(drive, buffer, number, lba);
    }
    return 1; // 成功
  } else {
    return 0; // 失败
  }
}
int get_vdisk_type(char drive) {
  int indx = drive - ('A');
  if (indx > 10) {
    return 0; // 失败
  }
  if (vdisk_ctl[indx].flag) {
    return vdisk_ctl[indx].flag;
  } else {
    return 0; // 失败
  }
}
bool have_vdisk(char drive) {
  int indx = drive - 'A';
  // printk("drive=%c\n",drive);
  if (indx > 10) {
    return 0; // 失败
  }
  if (vdisk_ctl[indx].flag) {
    return 1; // 成功
  } else {
    return 0; // 失败
  }
}
// 基于vdisk的通用读写
#define SECTORS_ONCE 8
void Disk_Read(u32 lba, u32 number, void *buffer, char drive) {
  if (have_vdisk(drive)) {
    for (int i = 0; i < number; i += SECTORS_ONCE) {
      int sectors = ((number - i) >= SECTORS_ONCE) ? SECTORS_ONCE : (number - i);
      // printf("Reading\n");
      rw_vdisk(drive, lba + i, buffer + i * 512, sectors, 1);
    }
  }
}
int disk_Size(char drive) {
  u8 drive1 = drive;
  if (have_vdisk(drive1)) {
    int indx = drive1 - 'A';
    return vdisk_ctl[indx].size;
  } else {
    printf("Disk Not Ready.\n");
    return 0;
  }

  return 0;
}
bool DiskReady(char drive) {
  return have_vdisk(drive);
}
int getReadyDisk() {
  return 0;
}
void Disk_Write(u32 lba, u32 number, void *buffer, char drive) {
  //  printf("%d\n",lba);
  if (have_vdisk(drive)) {
    // printk("*buffer(%d %d) = %02x\n",lba,number,*(u8 *)buffer);
    for (int i = 0; i < number; i += SECTORS_ONCE) {
      int sectors = ((number - i) >= SECTORS_ONCE) ? SECTORS_ONCE : (number - i);
      rw_vdisk(drive, lba + i, buffer + i * 512, sectors, 0);
    }
  }
}
