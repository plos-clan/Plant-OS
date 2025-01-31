#include <pl2d.hpp>

namespace pl2d {

// --------------------------------------------------
//. 简单的绘图函数

template <typename T>
auto BaseTexture<T>::line(const Point2I &p1, const Point2I &p2, const T &color) -> BaseTexture & {
  for (val[x, y] : ItPoint2I(p1, p2)) {
    at(x, y) = color;
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::line_mix(const Point2I &p1, const Point2I &p2, const T &color)
    -> BaseTexture & {
  for (val[x, y] : ItPoint2I(p1, p2)) {
    at(x, y).mix(color);
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::line(i32 x1, i32 y1, i32 x2, i32 y2, const T &color) -> BaseTexture & {
  for (val[x, y] : ItPoint2I(x1, y1, x2, y2)) {
    at(x, y) = color;
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::line_mix(i32 x1, i32 y1, i32 x2, i32 y2, const T &color) -> BaseTexture & {
  for (val[x, y] : ItPoint2I(x1, y1, x2, y2)) {
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
auto vectorize BaseTexture<T>::fill(const T &color) -> BaseTexture & {
  for (usize y = 0; y < height; y++) {
    for (usize x = 0; x < width; x++) {
      pixels[y * pitch + x] = color;
    }
  }
  return *this;
}

template <typename T>
auto vectorize BaseTexture<T>::fill(RectI rect, const T &color) -> BaseTexture & {
  rect.trunc(size_rect());
  for (usize y = rect.y1; y <= rect.y2; y++) {
    for (usize x = rect.x1; x <= rect.x2; x++) {
      pixels[y * pitch + x] = color;
    }
  }
  return *this;
}

template <typename T>
auto vectorize BaseTexture<T>::fill_mix(RectI rect, const T &color) -> BaseTexture & {
  for (usize y = 0; y < height; y++) {
    for (usize x = 0; x < width; x++) {
      pixels[y * pitch + x].mix(color);
    }
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::fill(const T &(*cb)(i32 x, i32 y)) -> BaseTexture & {
  for (val[x, y] : size_rect()) {
    at(x, y) = cb(x, y);
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::fill_trangle(const Point2I &p1, const Point2I &p2, const Point2I &p3,
                                  const T &color) -> BaseTexture & {
  static lit val edge_function = [](const Point2I &a, const Point2I &b, const Point2I &c) -> i32 {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
  };

  const Point2I min_point = {cpp::min(p1.x, p2.x, p3.x), cpp::min(p1.y, p2.y, p3.y)};
  const Point2I max_point = {cpp::max(p1.x, p2.x, p3.x), cpp::max(p1.y, p2.y, p3.y)};

  for (i32 y = min_point.y; y <= max_point.y; y++) {
    vectorize for (i32 x = min_point.x; x <= max_point.x; x++) {
      val p  = Point2I(x, y);
      val w0 = edge_function(p2, p3, p);
      val w1 = edge_function(p3, p1, p);
      val w2 = edge_function(p1, p2, p);
      if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) { //
        at(x, y) = color;
      }
    }
  }
  return *this;
}

BaseTextureInstantiation

} // namespace pl2d
