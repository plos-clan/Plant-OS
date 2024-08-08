#pragma once
#include <define.h>

#define ARGS_NOT_IN_STACK 1

typedef struct args {
  cstr   path;     // 路径
  cstr   cmdline;  // 命令
  cstr   argline;  // 参数
  cstr   envline;  // 环境变量
  cstr  *old_envp; // 旧环境变量
  void  *sp;       // 栈指针
  int    argc;     // 参数个数
  char **argv;     // 参数列表
  char **envp;     // 环境变量
} *args_t;

dlimport void *parse_args(args_t args);
