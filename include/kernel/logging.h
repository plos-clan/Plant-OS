#pragma once
#include <libc-base.h>

#define LOG_EXT 0 // 启用扩展的日志功能

#include "logging/undef"

#if LOG_EXT
#  include "logging/ext"
#else
#  include "logging/default"
#endif

void kenel_debugger_tick();
