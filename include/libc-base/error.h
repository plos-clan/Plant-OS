#pragma once
#include "00-include.h"

extern u32  error_num;
extern cstr error_msg;

// 返回值为整数的函数 负数表示错误
#define ERROR(id, msg)                                                                             \
  do {                                                                                             \
    error_msg = (msg);                                                                             \
    i32 __id  = (id);                                                                              \
    error_num = __id < 0 ? -__id : __id;                                                           \
    return -error_num;                                                                             \
  } while (0)
// 返回值为指针的函数 null 表示错误
#define ERROR_P(id, msg)                                                                           \
  do {                                                                                             \
    error_msg = (msg);                                                                             \
    i32 __id  = (id);                                                                              \
    error_num = __id < 0 ? -__id : __id;                                                           \
    return null;                                                                                   \
  } while (0)
