// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "00-include.hpp"
#include "type_traits.hpp"

class RefCount {
public:
  size_t __rc__ = 0;
};

template <typename T>
class __P__ {
#define __rc__ (((RefCount *)ptr)->__rc__)

public:
  T *ptr = null;

  __P__() = default;

  __P__(T *p) : ptr(p) {
    if (ptr) __rc__++;
  }

  template <typename U, typename = typename cpp::enable_if<cpp::is_base_of<T, U>>>
  __P__(U *p) : ptr(static_cast<T *>(p)) {
    if (ptr) __rc__++;
  }

  __P__(const __P__<T> &p) : ptr(p.ptr) {
    if (ptr) __rc__++;
  }

  template <typename U, typename = typename cpp::enable_if<cpp::is_base_of<T, U>>>
  __P__(const __P__<U> &p) : ptr(static_cast<T *>(p.ptr)) {
    if (ptr) __rc__++;
  }

  __P__(__P__<T> &&p) noexcept : ptr(p.ptr) {
    p.ptr = null;
  }

  template <typename U, typename = typename cpp::enable_if<cpp::is_base_of<T, U>>>
  __P__(__P__<U> &&p) noexcept : ptr(p.ptr) {
    p.ptr = null;
  }

  auto operator=(const __P__<T> &p) -> __P__<T> & {
    if (this == &p) return *this;
    if (ptr != null && --__rc__ == 0) delete ptr;
    ptr = p.ptr;
    __rc__++;
    return *this;
  }

  template <typename U, typename = typename cpp::enable_if<cpp::is_base_of<T, U>>>
  auto operator=(const __P__<U> &p) -> __P__<T> & {
    if (this == &p) return *this;
    if (ptr != null && --__rc__ == 0) delete ptr;
    ptr = static_cast<T *>(p.ptr);
    __rc__++;
    return *this;
  }

  auto operator=(__P__<T> &&p) noexcept -> __P__<T> & {
    if (this == &p) return *this;
    if (ptr != null && --__rc__ == 0) delete ptr;
    ptr   = p.ptr;
    p.ptr = null;
    return *this;
  }

  template <typename U, typename = typename cpp::enable_if<cpp::is_base_of<T, U>>>
  auto operator=(__P__<U> &&p) noexcept -> __P__<T> & {
    if (this == &p) return *this;
    if (ptr != null && --__rc__ == 0) delete ptr;
    ptr   = static_cast<T *>(p.ptr);
    p.ptr = null;
    return *this;
  }

  ~__P__() {
    if (ptr != null && --__rc__ == 0) delete ptr;
  }

  auto operator*() const -> T & {
    return *ptr;
  }

  auto operator->() const -> T * {
    return ptr;
  }

  operator T *() const {
    return ptr;
  }

  template <typename U, typename = typename cpp::enable_if<cpp::is_base_of<U, T>>>
  operator U *() const {
    return static_cast<U *>(ptr);
  }

#undef __rc__
};

#define __Pclass__(__name__)                                                                       \
  class __name__;                                                                                  \
  using P##__name__ = __P__<__name__>
#define __P__(__name__) using P##__name__ = __P__<__name__>

template <typename T1, typename T2>
static inline auto isinstance(const T2 *ptr) -> bool {
  static_assert(cpp::is_base_of<T2, T1>);
  return dynamic_cast<const T1 *>(ptr) != null;
}
