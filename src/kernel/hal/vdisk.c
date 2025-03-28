// This code is released under the MIT License

#include <kernel.h>

vdisk vdisk_ctl[26];

int vdisk_init() {
  for (int i = 0; i < 26; i++) {
    vdisk_ctl[i].flag = 0; // 设置为未使用
  }
  printi("ok");
  return 0;
}

int regist_vdisk(vdisk vd) {
  for (int i = 0; i < 26; i++) {
    if (!vdisk_ctl[i].flag) {
      vdisk_ctl[i] = vd;   // 找到了！
      devfs_regist_dev(i); // 注册设备
      return i;            // 注册成功，返回drive
    }
  }
  printe("not found\n");
  return 0; // 注册失败
}

int logout_vdisk(int drive) {
  int indx = drive;
  if (indx > 26) {
    return 0; // 失败
  }
  if (vdisk_ctl[indx].flag) {
    vdisk_ctl[indx].flag = 0; // 设置为没有
    return 1;                 // 成功
  } else {
    return 0; // 失败
  }
}

int rw_vdisk(int drive, u32 lba, u8 *buffer, u32 number, int read) {
  int indx = drive;
  if (indx > 26) {
    return 0; // 失败
  }
  if (vdisk_ctl[indx].flag) {
    if (read) {
      vdisk_ctl[indx].read(drive, buffer, number, lba);
    } else {
      vdisk_ctl[indx].write(drive, buffer, number, lba);
    }
    return 1; // 成功
  } else {
    return 0; // 失败
  }
}

bool have_vdisk(int drive) {
  int indx = drive;
  // printk("drive=%c\n",drive);
  if (indx > 26) {
    return 0; // 失败
  }
  if (vdisk_ctl[indx].flag) {
    return 1; // 成功
  } else {
    return 0; // 失败
  }
}
// 基于vdisk的通用读写

static u8 *drive_name[16] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static struct cir_queue8 drive_fifo[16];

static u8 drive_buf[16][256];

bool set_drive(u8 *name) {
  for (int i = 0; i != 16; i++) {
    if (drive_name[i] == NULL) {
      drive_name[i] = name;
      cir_queue8_init(&drive_fifo[i], 256, drive_buf[i]);
      return true;
    }
  }
  return false;
}

u32 get_drive_code(u8 *name) {
  for (int i = 0; i < 16; i++) {
    if (streq((cstr)drive_name[i], (cstr)name)) return i;
  }
  return 16;
}

bool drive_semaphore_take(u32 drive_code) {
  if (drive_code >= 16) { return true; }
  cir_queue8_put(&drive_fifo[drive_code], current_task->tid);
  // printk("FIFO: %d PUT: %d STATUS: %d\n", drive_code, Get_Tid(current_task),
  //        fifo8_status(&drive_fifo[drive_code]));
  while (drive_buf[drive_code][drive_fifo[drive_code].head] != current_task->tid) {
    // printk("Waiting....\n");
  }
  return true;
}

void drive_semaphore_give(u32 drive_code) {
  if (drive_code >= 16) { return; }
  if (drive_buf[drive_code][drive_fifo[drive_code].head] != current_task->tid) {
    // 暂时先不做处理 一般不会出现这种情况
    return;
  }
  cir_queue8_get(&drive_fifo[drive_code]);
}

#define SECTORS_ONCE 8
void vdisk_read(u32 lba, u32 number, void *buffer, int drive) {
  if (have_vdisk(drive)) {
    if (drive_semaphore_take(get_drive_code((u8 *)"DISK_DRIVE"))) {
      // printk("*buffer(%d %d) = %02x\n",lba,number,*(u8 *)buffer);
      for (int i = 0; i < number; i += SECTORS_ONCE) {
        int sectors = ((number - i) >= SECTORS_ONCE) ? SECTORS_ONCE : (number - i);
        rw_vdisk(drive, lba + i, (u8 *)((u32)buffer + i * vdisk_ctl[drive].sector_size), sectors,
                 1);
      }
      drive_semaphore_give(get_drive_code((u8 *)"DISK_DRIVE"));
    }
  }
}

u32 disk_size(int drive) {
  u8 drive1 = drive;
  if (have_vdisk(drive1)) {
    int indx = drive1;
    return vdisk_ctl[indx].size;
  } else {
    klog("Disk Not Ready.\n");
    return 0;
  }

  return 0;
}

bool disk_ready(int drive) {
  return have_vdisk(drive);
}

void vdisk_write(u32 lba, u32 number, const void *buffer, int drive) {
  //  printk("%d\n",lba);
  if (have_vdisk(drive)) {
    if (drive_semaphore_take(get_drive_code((u8 *)"DISK_DRIVE"))) {
      // printk("*buffer(%d %d) = %02x\n",lba,number,*(u8 *)buffer);
      for (int i = 0; i < number; i += SECTORS_ONCE) {
        int sectors = ((number - i) >= SECTORS_ONCE) ? SECTORS_ONCE : (number - i);
        rw_vdisk(drive, lba + i, (u8 *)((u32)buffer + i * vdisk_ctl[drive].sector_size), sectors,
                 0);
      }
      drive_semaphore_give(get_drive_code((u8 *)"DISK_DRIVE"));
    }
  }
}
