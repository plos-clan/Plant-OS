// 并没有任务调度，但是为了兼容。
#pragma once
struct TASK {
  int           drive_number;
  char          drive;
  struct vfs_t *nfs;
} __attribute__((packed));
#define vfs_now NowTask()->nfs
struct TASK *NowTask();