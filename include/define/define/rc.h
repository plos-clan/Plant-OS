// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "00-include.h"

// 与 include/libc-base/rc.hpp 行为相同

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
