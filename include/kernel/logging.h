#pragma once
#include <libc-base.h>

#include "logging/undef"

#if PLOS_LOGGING == 0
// 关闭日志
#  include "logging/default"
#elif PLOS_LOGGING == 1
// 终端日志
#  include "logging/default"
#elif PLOS_LOGGING == 2
// 终端日志 + 调试 shell
#  include "logging/default"
#elif PLOS_LOGGING == 3
// 扩展日志
#  include "logging/ext"
#else
#  error "Invalid PLOS_LOGGING value"
#endif

void kenel_debugger_tick();

void panic();

extern void *kernel_data;
