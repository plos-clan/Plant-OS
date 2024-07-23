#pragma once
#ifndef NO_CONFIG
#  include <config.h>
#endif

#define __PACKED__ __attribute__((packed)) // plos kernel 使用

#if !__BYTE_ORDER__ || !__ORDER_LITTLE_ENDIAN__ || !__ORDER_BIG_ENDIAN__
#  error "请指定端序"
#endif
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
#  error "端序必须为大端序或小端序"
#endif

#define _rest __restrict

#undef __THROW
#undef __wur
#undef __nonnull
#define __THROW           __attribute__((nothrow, leaf))
#define __wur             __attribute__((warn_unused_result))
#define __nonnull(params) __attribute__((nonnull params))

typedef __builtin_va_list va_list;
#define va_start(ap, para) __builtin_va_start(ap, para)
#define va_end(ap)         __builtin_va_end(ap)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_copy(dest, src) __builtin_va_copy(dest, src)

// __attribute__((overloadable)) 是 clang 扩展，使 C 函数可以被重载

#ifdef __cplusplus
#  define overload
#  define dlexport    __attribute__((visibility("default")))
#  define dlimport    extern
#  define dlimport_c  extern "C"
#  define dlimport_x  extern
#  define dlhidden    __attribute__((visibility("hidden")))
#  define dlinternal  __attribute__((visibility("internal")))
#  define dlprotected __attribute__((visibility("protected")))
#else
#  define overload    __attribute__((overloadable))
#  define dlexport    __attribute__((visibility("default")))
#  define dlimport    extern
#  define dlimport_c  extern
#  define dlimport_x  extern overload
#  define dlhidden    __attribute__((visibility("hidden")))
#  define dlinternal  __attribute__((visibility("internal")))
#  define dlprotected __attribute__((visibility("protected")))
#endif
#if DEBUG
#  define finline static
#else
#  define finline static inline __attribute__((always_inline))
#endif

// 获取数组的长度
#ifndef lengthof
#  define lengthof(arr) (sizeof(arr) / sizeof(*(arr)))
#endif

// 获取表达式的类型，类似于 auto
#define typeof(arg) __typeof__((void)0, arg)

#if NO_BUILTIN
#  define __has(name) (0)
#else
#  define __has(name) (__has_builtin(__builtin_##name))
#endif

#define CONCAT_(a, b) a##b
#define CONCAT(a, b)  CONCAT_(a, b)

#define CRGB(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"
#define CEND          "\033[0m"

#define COLOR_DEBUG CRGB(64, 128, 255)
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
#define STR_LOGINFO_FILE "[" CRGB(192, 64, 255) "%s" CEND "] "
#define ARG_LOGINFO_FUNC __func__, __LINE__
#define STR_LOGINFO_FUNC "[" CRGB(128, 192, 255) "%s" CEND ":" CRGB(192, 64, 192) "%d" CEND "] "

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

#define __ref(obj)                                                                                 \
  ({                                                                                               \
    typeof(obj) __obj = (obj);                                                                     \
    if (__obj == null) {                                                                           \
      printw("空对象");                                                                            \
    } else if (__obj->rc < 1) {                                                                    \
      printw("对象 %p 引用计数为 %d", __obj, __obj->rc);                                           \
      __obj = null;                                                                                \
    } else {                                                                                       \
      __obj->rc++;                                                                                 \
      printd("引用 %p 总计 %d", __obj, __obj->rc);                                                 \
    }                                                                                              \
    __obj;                                                                                         \
  })
#define __unref(obj, _free)                                                                        \
  ((void)({                                                                                        \
    typeof(obj) __obj = (obj);                                                                     \
    if (__obj == null) {                                                                           \
      printw("空对象");                                                                            \
    } else if (__obj->rc == 1) {                                                                   \
      _free(__obj);                                                                                \
      __obj = null;                                                                                \
    } else if (__obj->rc < 1) {                                                                    \
      printw("对象 %p 引用计数为 %d", __obj, __obj->rc);                                           \
      __obj = null;                                                                                \
    } else {                                                                                       \
      __obj->rc--;                                                                                 \
      printd("解引用 %p 总计 %d", __obj, __obj->rc);                                               \
    }                                                                                              \
  }))

#define _rc_alloc(_type, _len)                                                                     \
  ({                                                                                               \
    size_t *__ptr = (size_t *)malloc((_len) * sizeof(_type) + sizeof(size_t));                     \
    if (__ptr != null) {                                                                           \
      __ptr[0]  = 1;                                                                               \
      __ptr    += 1;                                                                               \
    }                                                                                              \
    (_type *)__ptr;                                                                                \
  })
#define _rc_free(_ptr)                                                                             \
  ({                                                                                               \
    size_t *__ptr = (size_t *)(_ptr);                                                              \
    if (__ptr != null) free(__ptr - 1);                                                            \
  })
#define _rc_refcnt(_ptr)                                                                           \
  ({                                                                                               \
    size_t  __rc  = 0;                                                                             \
    size_t *__ptr = (size_t *)(_ptr);                                                              \
    if (__ptr != null) __rc = __ptr[-1];                                                           \
    __rc;                                                                                          \
  })
#define _rc_ref(_ptr)                                                                              \
  ({                                                                                               \
    size_t  __rc  = 0;                                                                             \
    size_t *__ptr = (size_t *)(_ptr);                                                              \
    if (__ptr != null) __rc = ++__ptr[-1];                                                         \
    __rc;                                                                                          \
  })
#define _rc_unref(_ptr)                                                                            \
  ({                                                                                               \
    size_t  __rc  = 0;                                                                             \
    size_t *__ptr = (size_t *)(_ptr);                                                              \
    if (__ptr != null) {                                                                           \
      __rc = --__ptr[-1];                                                                          \
      if (__rc == 0) free(__ptr - 1);                                                              \
    }                                                                                              \
    __rc;                                                                                          \
  })

#ifdef __cplusplus
#  define assert_type_is_int(T) static_assert(std::is_integral_v<T>, "类型 " #T " 必须是整数")
#  define assert_type_is_float(T)                                                                  \
    static_assert(std::is_floating_point_v<T>, "类型 " #T " 必须是浮点")
#  define assert_type_is_ptr(T)   static_assert(std::is_pointer_v<T>, "类型 " #T " 必须是指针")
#  define assert_type_is_class(T) static_assert(std::is_class_v<T>, "类型 " #T " 必须是类")
#  define assert_type_is_num(T)                                                                    \
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, #T " 必须是数字")
#endif

#if STD_SAFE_API
#  define __std_safe__(code) ((void)({code}))
#else
#  define __std_safe__(code) ((void)(0))
#endif

#if SAFE_API
#  define __safe__(code) ((void)({code}))
#else
#  define __safe__(code) ((void)(0))
#endif
