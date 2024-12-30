#pragma once
#include <define.h>

//* ----------------------------------------------------------------------------------------------------
//; 这个文件可以被用户程序引用
//* ----------------------------------------------------------------------------------------------------

#define KERNEL_SHARED_BEGIN 0xf0000000

#define TASK_ARGS_ADDR      (KERNEL_SHARED_BEGIN + (0 * PAGE_SIZE))
#define TASK_CMDLINE_ADDR   (KERNEL_SHARED_BEGIN + (1 * PAGE_SIZE))
#define TASK_USER_PART_ADDR (KERNEL_SHARED_BEGIN + (2 * PAGE_SIZE))
