#pragma once
#include "00-include.h"
#include <define.h>

#define MAX_IPC_MESSAGE 5 // 一次最多存放5个IPC_MESSAGE
#define synchronous     1
#define asynchronous    2

typedef struct {
  void *data;
  u32   size;
  int   from_tid;
  int   flag1;
  int   flag2;
} IPCMessage;

typedef struct IPC_Header { // IPC头（在TASK结构体中的头）
  int        now;
  IPCMessage messages[MAX_IPC_MESSAGE];
  lock_t     l;
} IPC_Header;
