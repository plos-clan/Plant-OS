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

#define __attr_malloc __attribute__((warn_unused_result, malloc))
#ifdef __clang__
#  define __attr_dealloc(func, nparam) __attr_malloc
#else
#  define __attr_dealloc(func, nparam) __attribute__((warn_unused_result, malloc(func, nparam)))
#endif

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
    logi("): Oops! Something is wrong with your Computer:");                                       \
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

#define __ARGN_PRIVATE__(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15,     \
                         _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29,     \
                         _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43,     \
                         _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57,     \
                         _58, _59, _60, _61, _62, _63, _64, n, ...)                                \
  n
#define __ARGN__(...)                                                                              \
  __ARGN_PRIVATE__(0, ##__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50,   \
                   49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
                   29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
                   9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
