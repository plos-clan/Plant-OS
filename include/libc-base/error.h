#pragma once
#include "00-include.h"

struct error {
  u32  num;       // 错误号
  cstr msg;       // 错误信息
  bool free_flag; // 是否需要释放 msg
};

extern struct error last_error;

// 返回值为整数的函数 负数表示错误
#define ERROR(_id, _msg)                                                                           \
  do {                                                                                             \
    last_error.msg       = (_msg);                                                                 \
    i32 __id             = (_id);                                                                  \
    last_error.num       = __id < 0 ? -__id : __id;                                                \
    last_error.free_flag = false;                                                                  \
    return -last_error.num;                                                                        \
  } while (0)
// 返回值为指针的函数 null 表示错误
#define ERROR_P(_id, _msg)                                                                         \
  do {                                                                                             \
    last_error.msg       = (_msg);                                                                 \
    i32 __id             = (_id);                                                                  \
    last_error.num       = __id < 0 ? -__id : __id;                                                \
    last_error.free_flag = false;                                                                  \
    return null;                                                                                   \
  } while (0)
