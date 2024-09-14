// This code is released under the MIT License

#pragma once
#include "00-include.h"

#define CRGB(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"
#define CEND          "\033[0m"

#define COLOR_DEBUG CRGB(128, 192, 255)
#define COLOR_INFO  CRGB(64, 192, 128)
#define COLOR_WARN  CRGB(255, 192, 0)
#define COLOR_ERROR CRGB(255, 128, 64)
#define COLOR_FATAL CRGB(255, 64, 64)

#define STR_DEBUG "[" COLOR_DEBUG "Debug" CEND "] "
#define STR_INFO  "[" COLOR_INFO "Info " CEND "] "
#define STR_WARN  "[" COLOR_WARN "Warn " CEND "] "
#define STR_ERROR "[" COLOR_ERROR "Error" CEND "] "
#define STR_FATAL "[" COLOR_FATAL "Fatal" CEND "] "

static const char *_log_basename_(const char *path) {
  static char name[128];
  int         i = 0;
  while (path[i])
    i++;
  for (i--; i >= 0; i--) {
    if (path[i] == '/' || path[i] == '\\') break;
  }
  return path + i + 1;
}

#define ARG_LOGINFO_FILE _log_basename_(__FILE__)
#define STR_LOGINFO_FILE "[" CRGB(192, 128, 255) "%s" CEND "] "
#define ARG_LOGINFO_FUNC __func__, __LINE__
#define STR_LOGINFO_FUNC "[" CRGB(0, 255, 255) "%s" CEND ":" CRGB(255, 128, 192) "%d" CEND "] "

#define ARG_LOGINFO ARG_LOGINFO_FILE, ARG_LOGINFO_FUNC
#define STR_LOGINFO STR_LOGINFO_FILE STR_LOGINFO_FUNC

#define _LOG(type, fmt, ...)                                                                       \
  printf(CONCAT(STR, type) STR_LOGINFO CONCAT(COLOR, type) fmt CEND "\n", ARG_LOGINFO,             \
         ##__VA_ARGS__)

#ifdef DEBUG
#  define printd(fmt, ...) _LOG(_DEBUG, fmt, ##__VA_ARGS__)
#else
#  define printd(fmt, ...) _LOG(_DEBUG, fmt, ##__VA_ARGS__)
#endif

#define printi(fmt, ...) _LOG(_INFO, fmt, ##__VA_ARGS__)
#define printw(fmt, ...) _LOG(_WARN, fmt, ##__VA_ARGS__)
#define printe(fmt, ...) _LOG(_ERROR, fmt, ##__VA_ARGS__)

#define info(fmt, ...)  printi(fmt, ##__VA_ARGS__)
#define warn(fmt, ...)  printw(fmt, ##__VA_ARGS__)
#define error(fmt, ...) printe(fmt, ##__VA_ARGS__)
#define fatal(fmt, ...)                                                                            \
  ({                                                                                               \
    _LOG(_FATAL, fmt, ##__VA_ARGS__);                                                              \
    abort();                                                                                       \
    __builtin_unreachable();                                                                       \
  })

#if defined(DEBUG) && !defined(NO_MALLOC_MESSAGE)
#  define malloc(size)                                                                             \
    ({                                                                                             \
      size_t __size = (size);                                                                      \
      void  *__ptr  = malloc(__size);                                                              \
      printd("分配对象 %p 大小 %ld", __ptr, __size);                                               \
      __ptr;                                                                                       \
    })
#  define free(ptr)                                                                                \
    ({                                                                                             \
      void *__ptr = (void *)(ptr);                                                                 \
      printd("释放对象 %p", __ptr);                                                                \
      free(__ptr);                                                                                 \
    })
#endif
