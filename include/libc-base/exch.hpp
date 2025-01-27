#pragma once
#include <define.hpp>

namespace cpp {

#if __cplusplus >= 202002L
template <typename T>
requires(std::is_fundamental_v<T>)
inline_const void exch(T &x, T &y) {
  T t = x;
  x   = y;
  y   = t;
}
template <typename T>
requires(std::is_pointer_v<T>)
inline_const void exch(T &x, T &y) {
  T t = x;
  x   = y;
  y   = t;
}
template <typename T>
requires(std::is_class_v<T>)
inline_const void exch(T &x, T &y) {
  x.exch(y);
}
template <typename T>
requires(std::is_fundamental_v<T>)
inline_const void exch_if(bool b, T &x, T &y) {
  if (!b) return;
  T t = x;
  x   = y;
  y   = t;
}
template <typename T>
requires(std::is_pointer_v<T>)
inline_const void exch_if(bool b, T &x, T &y) {
  if (!b) return;
  T t = x;
  x   = y;
  y   = t;
}
template <typename T>
requires(std::is_class_v<T>)
inline_const void exch_if(bool b, T &x, T &y) {
  if (!b) return;
  x.exch(y);
}
#else
template <typename T>
inline_const auto exch(T &x, T &y) -> typename std::enable_if_t<std::is_fundamental_v<T>, void> {
  T t = x;
  x   = y;
  y   = t;
}
template <typename T>
inline_const auto exch(T &x, T &y) -> typename std::enable_if_t<std::is_pointer_v<T>, void> {
  T t = x;
  x   = y;
  y   = t;
}
template <typename T>
inline_const auto exch(T &x, T &y) -> typename std::enable_if_t<std::is_class_v<T>, void> {
  x.exch(y);
}
template <typename T>
inline_const auto exch_if(bool b, T &x, T &y) ->
    typename std::enable_if_t<std::is_fundamental_v<T>, void> {
  if (!b) return;
  T t = x;
  x   = y;
  y   = t;
}
template <typename T>
inline_const auto exch_if(bool b, T &x, T &y) ->
    typename std::enable_if_t<std::is_pointer_v<T>, void> {
  if (!b) return;
  T t = x;
  x   = y;
  y   = t;
}
template <typename T>
inline_const auto exch_if(bool b, T &x, T &y) ->
    typename std::enable_if_t<std::is_class_v<T>, void> {
  if (!b) return;
  x.exch(y);
}
#endif

} // namespace cpp
