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
auto BaseTexture<T>::trangle(const Point2I &p1, const Point2I &p2, const Point2I &p3,
                             const T &color) -> BaseTexture & {
  line(p1, p2, color);
  line(p2, p3, color);
  line(p3, p1, color);
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

template <typename T>
auto BaseTexture<T>::fill_trangle(const Point2I &p1, const Point2I &p2, const Point2I &p3,
                                  const T &color) -> BaseTexture & {
  const auto edge_function = [](const Point2I &a, const Point2I &b, const Point2I &c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
  };

  Point2I min_point = {cpp::min(p1.x, p2.x, p3.x), cpp::min(p1.y, p2.y, p3.y)};
  Point2I max_point = {cpp::max(p1.x, p2.x, p3.x), cpp::max(p1.y, p2.y, p3.y)};

  for (i32 y = min_point.y; y <= max_point.y; y++) {
    for (i32 x = min_point.x; x <= max_point.x; x++) {
      Point2I p  = {x, y};
      i32     w0 = edge_function(p2, p3, p);
      i32     w1 = edge_function(p3, p1, p);
      i32     w2 = edge_function(p1, p2, p);
      if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) { //
        at(x, y) = color;
      }
    }
  }
  return *this;
}

BaseTextureInstantiation

} // namespace pl2d
