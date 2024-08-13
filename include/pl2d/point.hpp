#pragma once
#include "matrix.hpp"

namespace pl2d {

template <typename T>
struct BasePoint2 {
  T x, y;

  auto operator+(const BasePoint2 &p) const -> BasePoint2 {
    return {x + p.x, y + p.y};
  }
  auto operator+(T d) const -> BasePoint2 {
    return {x + d, y + d};
  }
  auto operator-(const BasePoint2 &p) const -> BasePoint2 {
    return {x - p.x, y - p.y};
  }
  auto operator-(T d) const -> BasePoint2 {
    return {x - d, y - d};
  }
  auto operator*(const BasePoint2 &p) const -> BasePoint2 {
    return {x * p.x, y * p.y};
  }
  auto operator*(T s) const -> BasePoint2 {
    return {x * s, y * s};
  }
  auto operator/(const BasePoint2 &p) const -> BasePoint2 {
    return {x / p.x, y / p.y};
  }
  auto operator/(T s) const -> BasePoint2 {
    return {x / s, y / s};
  }

  auto operator+=(const BasePoint2 &p) -> BasePoint2 & {
    x += p.x;
    y += p.y;
    return *this;
  }
  auto operator+=(T d) -> BasePoint2 & {
    x += d;
    y += d;
    return *this;
  }
  auto operator-=(const BasePoint2 &p) -> BasePoint2 & {
    x -= p.x;
    y -= p.y;
    return *this;
  }
  auto operator-=(T d) -> BasePoint2 & {
    x -= d;
    y -= d;
    return *this;
  }
  auto operator*=(const BasePoint2 &p) -> BasePoint2 & {
    x *= p.x;
    y *= p.y;
    return *this;
  }
  auto operator*=(T s) -> BasePoint2 & {
    x *= s;
    y *= s;
    return *this;
  }
  auto operator/=(const BasePoint2 &p) -> BasePoint2 & {
    x /= p.x;
    y /= p.y;
    return *this;
  }
  auto operator/=(T s) -> BasePoint2 & {
    x /= s;
    y /= s;
    return *this;
  }

  auto dot(const BasePoint2 &p) const -> T {
    return x * p.x + y * p.y;
  }

  auto mod() const -> T {
    return cpp::sqrt(x * x + y * y);
  }

  auto trans(const Matrix2 &m) const -> BasePoint2 {
    T _x = m.xx * x + m.yx * y + m.dx;
    T _y = m.xy * x + m.yy * y + m.dy;
    return BasePoint2{_x, _y};
  }

  auto apply(const Matrix2 &m) -> BasePoint2 & {
    T _x = m.xx * x + m.yx * y + m.dx;
    T _y = m.xy * x + m.yy * y + m.dy;
    x    = _x;
    y    = _y;
    return *this;
  }

  auto operator*(const Matrix2 &m) const -> BasePoint2 {
    return trans(m);
  }
  friend auto operator*(const Matrix2 &m, const BasePoint2 &p) -> BasePoint2 {
    return p.trans(m);
  }
  auto operator*=(const Matrix2 &m) -> BasePoint2 {
    return apply(m);
  }
};
template <typename T>
struct BasePoint3 {
  T x, y, z;

  auto operator+(const BasePoint3 &p) const -> BasePoint3 {
    return {x + p.x, y + p.y, z + p.z};
  }
  auto operator+(T d) const -> BasePoint3 {
    return {x + d, y + d, z + d};
  }
  auto operator-(const BasePoint3 &p) const -> BasePoint3 {
    return {x - p.x, y - p.y, z - p.z};
  }
  auto operator-(T d) const -> BasePoint3 {
    return {x - d, y - d, z - d};
  }
  auto operator*(const BasePoint3 &p) const -> BasePoint3 {
    return {x * p.x, y * p.y, z * p.z};
  }
  auto operator*(T s) const -> BasePoint3 {
    return {x * s, y * s, z * s};
  }
  auto operator/(const BasePoint3 &p) const -> BasePoint3 {
    return {x / p.x, y / p.y, z / p.z};
  }
  auto operator/(T s) const -> BasePoint3 {
    return {x / s, y / s, z / s};
  }

  auto operator+=(const BasePoint3 &p) -> BasePoint3 & {
    x += p.x;
    y += p.y;
    z += p.z;
    return *this;
  }
  auto operator+=(T d) -> BasePoint3 & {
    x += d;
    y += d;
    z += d;
    return *this;
  }
  auto operator-=(const BasePoint3 &p) -> BasePoint3 & {
    x -= p.x;
    y -= p.y;
    z -= p.z;
    return *this;
  }
  auto operator-=(T d) -> BasePoint3 & {
    x -= d;
    y -= d;
    z -= d;
    return *this;
  }
  auto operator*=(const BasePoint3 &p) -> BasePoint3 & {
    x *= p.x;
    y *= p.y;
    z *= p.z;
    return *this;
  }
  auto operator*=(T s) -> BasePoint3 & {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }
  auto operator/=(const BasePoint3 &p) -> BasePoint3 & {
    x /= p.x;
    y /= p.y;
    z /= p.z;
    return *this;
  }
  auto operator/=(T s) -> BasePoint3 & {
    x /= s;
    y /= s;
    z /= s;
    return *this;
  }

  auto dot(const BasePoint3 &p) const -> T {
    return x * p.x + y * p.y + z * p.z;
  }

  auto mod() const -> T {
    return cpp::sqrt(x * x + y * y + z * z);
  }

  auto trans(const Matrix3 &m) const -> BasePoint3 {
    T _x = m.xx * x + m.yx * y + m.zx * z + m.dx;
    T _y = m.xy * x + m.yy * y + m.zy * z + m.dy;
    T _z = m.xz * x + m.yz * y + m.zz * z + m.dz;
    return BasePoint3{_x, _y, _z};
  }

  auto apply(const Matrix3 &m) -> BasePoint3 & {
    T _x = m.xx * x + m.yx * y + m.zx * z + m.dx;
    T _y = m.xy * x + m.yy * y + m.zy * z + m.dy;
    T _z = m.xz * x + m.yz * y + m.zz * z + m.dz;
    x    = _x;
    y    = _y;
    z    = _z;
    return *this;
  }

  auto operator*(const Matrix3 &m) const -> BasePoint3 {
    return trans(m);
  }
  friend auto operator*(const Matrix3 &m, const BasePoint3 &p) -> BasePoint3 {
    return p.trans(m);
  }
  auto operator*=(const Matrix3 &m) -> BasePoint3 & {
    return apply(m);
  }
};

using Point2I = BasePoint2<i32>;
using Point2F = BasePoint2<f32>;
using Point2D = BasePoint2<f64>;
using Point3I = BasePoint3<i32>;
using Point3F = BasePoint3<f32>;
using Point3D = BasePoint3<f64>;
using Point2  = Point2F;
using Point3  = Point3F;
using Point   = Point2;

class ItPoint2I {
private:
  Point2I p1;
  Point2I p2;

public:
  ItPoint2I(Point2I begin, Point2I end) : p1(begin), p2(end) {}
  ItPoint2I(i32 x1, i32 y1, i32 x2, i32 y2) : p1{x1, y1}, p2{x2, y2} {}

  class Iterator {
  private:
    bool ended = false;
    i32  x1, y1, x2, y2;
    i32  dx, dy, sx, sy, err;

  public:
    Iterator() : ended(true) {}
    Iterator(Point2I p1, Point2I p2) {
      x1 = p1.x;
      y1 = p1.y;
      x2 = p2.x;
      y2 = p2.y;

      dx  = cpp::abs(x2 - x1);
      dy  = cpp::abs(y2 - y1);
      sx  = (x1 < x2) ? 1 : -1;
      sy  = (y1 < y2) ? 1 : -1;
      err = dx - dy;
    }

    auto operator*() -> Point2I {
      return {x1, y1};
    }

    auto operator++() -> Iterator & {
      if (ended) return *this;
      if (x1 == x2 && y1 == y2) {
        ended = true;
        return *this;
      }

      if (2 * err > -dy) {
        err -= dy;
        x1  += sx;
      }
      if (2 * err < dx) {
        err += dx;
        y1  += sy;
      }
      return *this;
    }

    auto operator==(const Iterator &it) const -> bool {
      return ended == it.ended;
    }

    auto operator!=(const Iterator &it) const -> bool {
      return ended != it.ended;
    }
  };

  auto begin() const -> Iterator {
    return {p1, p2};
  }

  auto end() const -> Iterator {
    return {};
  }
};

} // namespace pl2d
