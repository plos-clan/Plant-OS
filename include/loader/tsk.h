// 并没有任务调度，但是为了兼容。
#pragma once
#include <define.h>

struct TASK {
  int           drive_number;
  char          drive;
  struct vfs_t *nfs;
} __PACKED__;
#define vfs_now NowTask()->nfs
struct TASK *NowTask();