#pragma once
#include <libc.h>

dlimport void logk_raw(cstr s);

dlimport int logk(cstr _rest fmt, ...);

#define __LOG(type, fmt, ...)                                                                      \
  logk(CONCAT(STR, type) STR_LOGINFO CONCAT(COLOR, type) fmt CEND "\n", ARG_LOGINFO, ##__VA_ARGS__)

#define logd(fmt, ...) __LOG(_DEBUG, fmt, ##__VA_ARGS__)
#define logi(fmt, ...) __LOG(_INFO, fmt, ##__VA_ARGS__)
#define logw(fmt, ...) __LOG(_WARN, fmt, ##__VA_ARGS__)
#define loge(fmt, ...) __LOG(_ERROR, fmt, ##__VA_ARGS__)
#define logf(fmt, ...) __LOG(_FATAL, fmt, ##__VA_ARGS__)
