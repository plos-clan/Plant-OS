// This code is released under the MIT License

#pragma once
#include "00-include.hpp"

// 与 include/define/define/rc.h 行为相同

namespace cpp::rc {

template <typename T>
finline auto alloc(size_t size) -> T * {
  var *ptr = (size_t *)malloc(size * sizeof(T) + sizeof(size_t));
  if (ptr != null) {
    ptr[0]  = 1;
    ptr    += 1;
  }
  return (T *)ptr;
}

template <typename T>
finline auto free(T *ptr) -> void {
  var *p = (size_t *)ptr;
  if (p != null) free(p - 1);
}

template <typename T>
finline auto refcnt(T *ptr) -> size_t {
  size_t rc = 0;
  var   *p  = (size_t *)ptr;
  if (p != null) rc = p[-1];
  return rc;
}

template <typename T>
finline auto ref(T *ptr) -> size_t {
  size_t rc = 0;
  var   *p  = (size_t *)ptr;
  if (p != null) rc = ++p[-1];
  return rc;
}

template <typename T>
finline auto unref(T *ptr) -> size_t {
  size_t rc = 0;
  var   *p  = (size_t *)ptr;
  if (p != null) {
    rc = --p[-1];
    if (rc == 0) free(p - 1);
  }
  return rc;
}

} // namespace cpp::rc
