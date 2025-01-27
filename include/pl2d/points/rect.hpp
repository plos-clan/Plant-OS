#pragma once
#include "../point.hpp"
#include <define.h>

namespace pl2d {

template <typename T>
struct BaseRect {
  T x1 = 0, y1 = 0; // 左上角坐标
  T x2 = 0, y2 = 0; // 右下角坐标

  INLINE_CONST BaseRect() = default;
  INLINE_CONST BaseRect(T w, T h) {
    this->x1 = 0, this->y1 = 0;
    this->x2 = w - 1, this->y2 = h - 1;
  }
  INLINE_CONST BaseRect(T x1, T y1, T x2, T y2) {
    cpp::exch_if(x1 > x2, x1, x2);
    cpp::exch_if(y1 > y2, y1, y2);
    this->x1 = x1, this->y1 = y1;
    this->x2 = x2, this->y2 = y2;
  }
  INLINE_CONST BaseRect(const BaseRect &)     = default;
  INLINE_CONST BaseRect(BaseRect &&) noexcept = default;

  auto operator=(const BaseRect &) -> BaseRect &     = default;
  auto operator=(BaseRect &&) noexcept -> BaseRect & = default;

  static auto from_points(BasePoint2<T> *points, size_t npoints) -> BaseRect && {
    T x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    for (size_t i = 0; i < npoints; i++) {
      x1 = cpp::min(x1, points[i].x);
      y1 = cpp::min(y1, points[i].y);
      x2 = cpp::max(x2, points[i].x);
      y2 = cpp::max(y2, points[i].y);
    }
    return {x1, y1, x2, y2};
  }

  // 矩形的宽度
  constexpr auto width() const noexcept -> size_t {
    return x2 - x1 + 1;
  }
  // 矩形的高度
  constexpr auto height() const noexcept -> size_t {
    return y2 - y1 + 1;
  }
  // 矩形的大小
  constexpr auto size() const noexcept -> size_t {
    return width() * height();
  }

  auto translate(T x, T y) -> BaseRect & {
    x1 += x;
    x2 += x;
    y1 += y;
    y2 += y;
    return *this;
  }

  auto topleft() const -> BasePoint2<T> {
    return {x1, y1};
  }
  auto topright() const -> BasePoint2<T> {
    return {x2, y1};
  }
  auto bottomleft() const -> BasePoint2<T> {
    return {x1, y2};
  }
  auto bottomright() const -> BasePoint2<T> {
    return {x2, y2};
  }

  // 给他裁剪到指定宽高
  auto trunc(T w, T h) -> BaseRect & {
    x1 = cpp::max(0, x1);
    y1 = cpp::max(0, y1);
    x2 = cpp::min(w - 1, x2);
    y2 = cpp::min(h - 1, y2);
    return *this;
  }
  // 裁剪到指定矩形
  auto trunc(const BaseRect &r) -> BaseRect & {
    x1 = cpp::max(r.x1, x1);
    y1 = cpp::max(r.y1, y1);
    x2 = cpp::min(r.x2, x2);
    y2 = cpp::min(r.y2, y2);
    return *this;
  }
  auto trunc(T x1, T y1, T x2, T y2) -> BaseRect & {
    return trunc({x1, y1, x2, y2});
  }

  // 调整矩形使其包含某个点
  void contain(T x, T y) {
    x1 = cpp::min(x1, x);
    y1 = cpp::min(y1, y);
    x2 = cpp::max(x2, x);
    y2 = cpp::max(y2, y);
  }
  // 调整矩形使其包含某个点
  void contain(const BasePoint2<T> &p) {
    contain(p.x, p.y);
  }

  // 检查矩形是否包含某个点
  auto contains(T x, T y) const -> bool {
    if (x < x1 || x > x2) return false;
    if (y < y1 || y > y2) return false;
    return true;
  }
  // 检查矩形是否包含某个点
  auto contains(const BasePoint2<T> &p) const -> bool {
    return contains(p.x, p.y);
  }

  // 迭代器
  class Iterator {
  private:
    i32 x1, y1, x2, y2;
    i32 x, y;

  public:
    Iterator() = delete;
    Iterator(i32 x1, i32 y1, i32 x2, i32 y2) : x1(x1), y1(y1), x2(x2), y2(y2), x(x1), y(y1) {}
    Iterator(i32 x1, i32 y1, i32 x2, i32 y2, i32 x, i32 y)
        : x1(x1), y1(y1), x2(x2), y2(y2), x(x), y(y) {}
    Iterator(const Iterator &)                         = default;
    Iterator(Iterator &&) noexcept                     = default;
    auto operator=(const Iterator &) -> Iterator &     = default;
    auto operator=(Iterator &&) noexcept -> Iterator & = default;

    auto operator*() -> Point2I {
      return {x, y};
    }

    auto operator++() -> Iterator & {
      x++;
      if (x > x2) {
        x = x1;
        y++;
      }
      return *this;
    }

    auto operator==(const Iterator &it) const -> bool {
      return y == it.y && x == it.x;
    }

    auto operator!=(const Iterator &it) const -> bool {
      return y != it.y || x != it.x;
    }
  };

  auto begin() const -> Iterator {
    return {x1, y1, x2, y2, x1, y1};
  }

  auto end() const -> Iterator {
    return {x1, y1, x2, y2, x1, y2 + 1};
  }
};

using RectI = BaseRect<i32>;
using RectF = BaseRect<f32>;
using RectD = BaseRect<f64>;
using Rect  = RectI;

} // namespace pl2d
