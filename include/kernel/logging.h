#pragma once
#include <libc-base.h>

dlimport void klog_raw(cstr s);

dlimport void klog(cstr _rest fmt, ...);

dlimport void log_printf(cstr _rest fmt1, cstr _rest fmt2, ...);

#undef STR_LOGINFO_FILE
#undef STR_LOGINFO_FUNC
#define STR_LOGINFO_FILE "[" CRGB(192, 128, 255) "%!8s" CEND "] "
#define STR_LOGINFO_FUNC "[" CRGB(0, 255, 255) "%!8s" CEND ":" CRGB(255, 128, 192) "%-5d" CEND "] "

#define __LOG(type, fmt, ...)                                                                      \
  klog(CONCAT(STR, type) STR_LOGINFO CONCAT(COLOR, type) fmt CEND "\n", ARG_LOGINFO, ##__VA_ARGS__)

#define klogd(fmt, ...) __LOG(_DEBUG, fmt, ##__VA_ARGS__)
#define klogi(fmt, ...) __LOG(_INFO, fmt, ##__VA_ARGS__)
#define klogw(fmt, ...) __LOG(_WARN, fmt, ##__VA_ARGS__)
#define kloge(fmt, ...) __LOG(_ERROR, fmt, ##__VA_ARGS__)
#define klogf(fmt, ...) __LOG(_FATAL, fmt, ##__VA_ARGS__)

#define _COLOR_DEBUG "\033[1;36m"
#define _COLOR_INFO  "\033[1;32m"
#define _COLOR_WARN  "\033[1;33m"
#define _COLOR_ERROR "\033[1;31m"
#define _COLOR_FATAL "\033[1;31m"

#define _STR_DEBUG "[" _COLOR_DEBUG "Debug" CEND "] "
#define _STR_INFO  "[" _COLOR_INFO "Info " CEND "] "
#define _STR_WARN  "[" _COLOR_WARN "Warn " CEND "] "
#define _STR_ERROR "[" _COLOR_ERROR "Error" CEND "] "
#define _STR_FATAL "[" _COLOR_FATAL "Fatal" CEND "] "

#define _STR_LOGINFO_FILE "[\033[1;35m%!8s" CEND "] "
#define _STR_LOGINFO_FUNC "[\033[1;36m%!8s" CEND ":\033[1;35m%-5d" CEND "] "
#define _STR_LOGINFO      _STR_LOGINFO_FILE _STR_LOGINFO_FUNC

#undef _LOG
#define _LOG(type, fmt, ...)                                                                       \
  log_printf(CONCAT(STR, type) STR_LOGINFO CONCAT(COLOR, type) fmt CEND "\n",                      \
             CONCAT(_STR, type) _STR_LOGINFO CONCAT(_COLOR, type) fmt CEND "\n", ARG_LOGINFO,      \
             ##__VA_ARGS__)

#undef fatal
#define fatal(fmt, ...)                                                                            \
  ({                                                                                               \
    _LOG(_INFO, "): Oops! Something is wrong with your Computer:");                                \
    _LOG(_FATAL, fmt, ##__VA_ARGS__);                                                              \
    abort();                                                                                       \
    __builtin_unreachable();                                                                       \
  })
