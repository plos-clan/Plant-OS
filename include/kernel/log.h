#pragma once
#include <libc.h>

dlimport void logk_raw(cstr s);

dlimport int logk(cstr _rest fmt, ...);

dlimport int log_printf(cstr _rest fmt, ...);

#undef __LOG
#define __LOG(type, fmt, ...)                                                                      \
  logk(CONCAT(STR, type) STR_LOGINFO CONCAT(COLOR, type) fmt CEND "\n", ARG_LOGINFO, ##__VA_ARGS__)

#define logd(fmt, ...) __LOG(_DEBUG, fmt, ##__VA_ARGS__)
#define logi(fmt, ...) __LOG(_INFO, fmt, ##__VA_ARGS__)
#define logw(fmt, ...) __LOG(_WARN, fmt, ##__VA_ARGS__)
#define loge(fmt, ...) __LOG(_ERROR, fmt, ##__VA_ARGS__)
#define logf(fmt, ...) __LOG(_FATAL, fmt, ##__VA_ARGS__)

#define _COLOR_DEBUG "\033[1;36m"
#define _COLOR_INFO  "\033[1;32m"
#define _COLOR_WARN  "\033[1;33m"
#define _COLOR_ERROR "\033[1;31m"
#define _COLOR_FATAL "\033[31m"

#define _STR_DEBUG "[" _COLOR_DEBUG "Debug" CEND "] "
#define _STR_INFO  "[" _COLOR_INFO "Info " CEND "] "
#define _STR_WARN  "[" _COLOR_WARN "Warn " CEND "] "
#define _STR_ERROR "[" _COLOR_ERROR "Error" CEND "] "
#define _STR_FATAL "[" _COLOR_FATAL "Fatal" CEND "] "

#define _STR_LOGINFO_FILE "[\033[1;35m%s" CEND "] "
#define _STR_LOGINFO_FUNC "[\033[1;36m%s" CEND ":\033[1;35m%d" CEND "] "
#define _STR_LOGINFO      _STR_LOGINFO_FILE _STR_LOGINFO_FUNC

#undef _LOG
#define _LOG(type, fmt, ...)                                                                       \
  ({                                                                                               \
    logk(CONCAT(STR, type) STR_LOGINFO CONCAT(COLOR, type) fmt CEND "\n", ARG_LOGINFO,             \
         ##__VA_ARGS__);                                                                           \
    log_printf(CONCAT(_STR, type) _STR_LOGINFO CONCAT(_COLOR, type) fmt CEND "\n", ARG_LOGINFO,    \
               ##__VA_ARGS__);                                                                     \
  })
