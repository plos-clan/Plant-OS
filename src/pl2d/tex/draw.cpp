#include <pl2d.hpp>

namespace pl2d {

// --------------------------------------------------
//. 简单的绘图函数

template <typename T>
auto BaseTexture<T>::line(const Point2I &p1, const Point2I &p2, const T &color) -> BaseTexture & {
  for (const auto [x, y] : ItPoint2I(p1, p2)) {
    at(x, y) = color;
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::line_mix(const Point2I &p1, const Point2I &p2, const T &color)
    -> BaseTexture & {
  for (const auto [x, y] : ItPoint2I(p1, p2)) {
    at(x, y).mix(color);
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::line(i32 x1, i32 y1, i32 x2, i32 y2, const T &color) -> BaseTexture & {
  for (const auto [x, y] : ItPoint2I(x1, y1, x2, y2)) {
    at(x, y) = color;
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::line_mix(i32 x1, i32 y1, i32 x2, i32 y2, const T &color) -> BaseTexture & {
  for (const auto [x, y] : ItPoint2I(x1, y1, x2, y2)) {
    at(x, y).mix(color);
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::fill(const T &color) -> BaseTexture & {
  for (const auto [x, y] : size_rect()) {
    at(x, y) = color;
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::fill(RectI rect, const T &color) -> BaseTexture & {
  for (const auto [x, y] : rect) {
    at(x, y) = color;
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::fill_mix(RectI rect, const T &color) -> BaseTexture & {
  for (const auto [x, y] : rect) {
    at(x, y).mix(color);
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::fill(const T &(*cb)(i32 x, i32 y)) -> BaseTexture & {
  for (const auto [x, y] : size_rect()) {
    at(x, y) = cb(x, y);
  }
  return *this;
}

BaseTextureInstantiation

} // namespace pl2d
