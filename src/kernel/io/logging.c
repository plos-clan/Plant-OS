#include <kernel.h>

// 一些用于日志记录和打印的函数。

#define PORT 0x3f8 // 日志输出端口：COM1

#if LOG_EXT
#  include "logging/ext.h"
#else
#  if PLOS_DEBUG_SHELL
#    include "logging/default.debug.h"
#  else
#    include "logging/default.h"
#  endif
#endif
