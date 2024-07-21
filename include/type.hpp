#pragma once
#include <config.h>

#include <type_traits>
// 实在实现不出来啊

namespace cpp {

template <typename T>
struct remove_reference {
  using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

template <typename T>
constexpr auto move(T &&t) noexcept -> remove_reference_t<T> && {
  return static_cast<remove_reference_t<T> &&>(t);
}

template <bool, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
  using type = T;
};

template <bool C, typename T = void>
using enable_if_t = typename enable_if<C, T>::type;

template <typename T, T v>
struct integral_constant {};

template <typename Base, typename Derived>
struct is_base_of : public integral_constant<bool, __is_base_of(Base, Derived)> {};

template <typename Base, typename Derived>
inline constexpr bool is_base_of_v = __is_base_of(Base, Derived);

} // namespace cpp

// 偷个懒，直接使用 C 的版本
#include <type.h>

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

  template <typename U, typename = typename cpp::enable_if_t<cpp::is_base_of_v<T, U>>>
  __P__(U *p) : ptr(static_cast<T *>(p)) {
    if (ptr) __rc__++;
  }

  __P__(const __P__<T> &p) : ptr(p.ptr) {
    if (ptr) __rc__++;
  }

  template <typename U, typename = typename cpp::enable_if_t<cpp::is_base_of_v<T, U>>>
  __P__(const __P__<U> &p) : ptr(static_cast<T *>(p.ptr)) {
    if (ptr) __rc__++;
  }

  __P__(__P__<T> &&p) noexcept : ptr(p.ptr) {
    p.ptr = null;
  }

  template <typename U, typename = typename cpp::enable_if_t<cpp::is_base_of_v<T, U>>>
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

  template <typename U, typename = typename cpp::enable_if_t<cpp::is_base_of_v<T, U>>>
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

  template <typename U, typename = typename cpp::enable_if_t<cpp::is_base_of_v<T, U>>>
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

  template <typename U, typename = typename cpp::enable_if_t<cpp::is_base_of_v<U, T>>>
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
  static_assert(cpp::is_base_of_v<T2, T1>);
  return dynamic_cast<const T1 *>(ptr) != null;
}
