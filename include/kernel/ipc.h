#pragma once
#include "00-include.h"
#include <define.h>

struct IPC_DATA {
  i32             pid;   // 发送者 ID
  i32             tpid;  // 接收者 ID
  i32             msgid; // 消息 ID
  struct timespec time;  // 发送时间
  void           *data;  // 数据指针
  size_t          size;  // 数据大小
};

/**
 *\brief 发送 IPC 消息
 *
 *\param pid      进程 ID，-1 为广播，其余负值无效
 *\param msgid    32 位有符号信息编号，负值无效
 *\param data     数据指针
 *\param size     数据大小
 *\param switch_task 是否切换任务
 */
void ipc_send(i32 pid, i32 msgid, const void *data, size_t size, bool switch_task);

/**
 *\brief 接收 IPC 消息
 *
 *\param msgid    32 位有符号信息编号指针，负值匹配所有
 *\param data     数据指针
 *\param size     数据大小
 *\param wait     是否等待
 *\return         发送者的进程 ID
 */
i32 ipc_recv(i32 msgid, void *data, size_t size, bool wait);

/**
 *\brief 唤醒进程
 *
 *\param pid         进程 ID
 *\param switch_task 是否切换任务
 */
void ipc_wakeup(i32 pid, bool switch_task);
