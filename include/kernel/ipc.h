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

/**
 *\brief 发送 IPC 消息
 *
 *\param pid      进程 ID，-1 为广播，其余负值无效
 *\param msgid    32 位有符号信息编号，负值无效
 *\param data     数据指针
 *\param size     数据大小
 */
void ipc_send(i32 pid, i32 msgid, const void *data, size_t size);

/**
 *\brief 接收 IPC 消息
 *
 *\param msgid    32 位有符号信息编号，负值无效
 *\param data     数据指针
 *\param size     数据大小
 *\return         发送者的进程 ID
 */
i32 ipc_recv(i32 msgid, void *data, size_t size);
