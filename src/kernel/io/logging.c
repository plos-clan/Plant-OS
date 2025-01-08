#include <kernel.h>

// 一些用于日志记录和打印的函数。

#define PORT 0x3f8 // 日志输出端口：COM1

#if PLOS_LOGGING == 0
// 关闭日志
#  include "logging/none.h"
#elif PLOS_LOGGING == 1
// 终端日志
#  include "logging/default.h"
#elif PLOS_LOGGING == 2
// 终端日志 + 调试 shell
#  include "logging/default.debug.h"
#elif PLOS_LOGGING == 3
// 扩展日志
#  include "logging/ext.h"
#else
#  error "Invalid PLOS_LOGGING value"
#endif
