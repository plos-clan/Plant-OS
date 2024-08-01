#pragma once
#include <define.h>
#include <type.hpp>

namespace cpp {

template <typename T>
auto exch(T &x, T &y) -> typename std::enable_if_t<std::is_fundamental_v<T>, void> {
  T t = x;
  x   = y;
  y   = t;
}

template <typename T>
auto exch(T &x, T &y) -> typename std::enable_if_t<std::is_pointer_v<T>, void> {
  T t = x;
  x   = y;
  y   = t;
}

template <typename T>
auto exch(T &x, T &y) -> typename std::enable_if_t<std::is_class_v<T>, void> {
  x.exch(y);
}

template <typename T>
auto exch_if(bool b, T &x, T &y) -> typename std::enable_if_t<std::is_fundamental_v<T>, void> {
  if (!b) return;
  T t = x;
  x   = y;
  y   = t;
}

template <typename T>
auto exch_if(bool b, T &x, T &y) -> typename std::enable_if_t<std::is_pointer_v<T>, void> {
  if (!b) return;
  T t = x;
  x   = y;
  y   = t;
}

template <typename T>
auto exch_if(bool b, T &x, T &y) -> typename std::enable_if_t<std::is_class_v<T>, void> {
  if (!b) return;
  x.exch(y);
}

} // namespace cpp
