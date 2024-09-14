#pragma once
#include <define.h>
#include <libc-base.hpp>

namespace pl2d {

struct Matrix2 {
  union {
    struct {
      float xx = 1, yx = 0;
      float xy = 0, yy = 1;
      float dx = 0, dy = 0;
    };
    float d[6];
  };

  auto operator[](size_t i) const -> float {
    return d[i];
  }
  auto operator[](size_t i) -> float & {
    return d[i];
  }

  auto operator==(const Matrix2 &m) const -> bool {
    for (size_t i = 0; i < 6; i++) {
      if (cpp::abs(d[i] - m[i]) > 1e-6) return false;
    }
    return true;
  }

  auto reset() -> Matrix2 & {
    *this = {1, 0, 0, 1, 0, 0};
    return *this;
  }

  auto translate(float tx, float ty) -> Matrix2 & {
    dx += tx;
    dy += ty;
    return *this;
  }

  auto scale(float s) -> Matrix2 & {
    xx *= s;
    yy *= s;
    return *this;
  }

  auto scale(float sx, float sy) -> Matrix2 & {
    xx *= sx;
    yy *= sy;
    return *this;
  }

  auto rotate(float a) -> Matrix2 & {
    float C   = cpp::cos(a);
    float S   = cpp::sin(a);
    float _xx = xx * C - yx * S;
    float _yx = xx * S + yx * C;
    float _xy = xy * C - yy * S;
    float _yy = xy * S + yy * C;
    xx        = _xx;
    yx        = _yx;
    xy        = _xy;
    yy        = _yy;
    return *this;
  }
};

struct Matrix3 {
  union {
    struct {
      float xx = 1, yx = 0, zx = 0;
      float xy = 0, yy = 1, zy = 0;
      float xz = 0, yz = 0, zz = 1;
      float dx = 0, dy = 0, dz = 0;
    };
    float d[12];
  };

  auto operator[](size_t i) const -> float {
    return d[i];
  }
  auto operator[](size_t i) -> float & {
    return d[i];
  }

  auto operator==(const Matrix3 &m) const -> bool {
    for (size_t i = 0; i < 12; i++) {
      if (cpp::abs(d[i] - m[i]) > 1e-6) return false;
    }
    return true;
  }

  auto reset() -> Matrix3 & {
    *this = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
    return *this;
  }

  auto translate(float tx, float ty, float tz) -> Matrix3 & {
    dx += tx;
    dy += ty;
    dz += tz;
    return *this;
  }

  auto scale(float s) -> Matrix3 & {
    xx *= s;
    yy *= s;
    zz *= s;
    return *this;
  }

  auto scale(float sx, float sy, float sz) -> Matrix3 & {
    xx *= sx;
    yy *= sy;
    zz *= sz;
    return *this;
  }

  auto rotate_x(float a) -> Matrix3 & {
    float C   = cpp::cos(a);
    float S   = cpp::sin(a);
    float _yx = yx * C - zx * S;
    float _yy = yx * S + zx * C;
    float _zx = zx * C + yz * S;
    float _zy = yz * C - zx * S;
    yx        = _yx;
    yy        = _yy;
    zx        = _zx;
    zy        = _zy;
    return *this;
  }

  auto rotate_y(float a) -> Matrix3 & {
    float C   = cpp::cos(a);
    float S   = cpp::sin(a);
    float _xx = xx * C + zx * S;
    float _xy = xy * C + zy * S;
    float _zx = zx * C - xx * S;
    float _zy = zy * C - xy * S;
    xx        = _xx;
    xy        = _xy;
    zx        = _zx;
    zy        = _zy;
    return *this;
  }

  auto rotate_z(float a) -> Matrix3 & {
    float C   = cpp::cos(a);
    float S   = cpp::sin(a);
    float _xx = xx * C - yx * S;
    float _xy = xy * C - yy * S;
    float _yx = xx * S + yx * C;
    float _yy = xy * S + yy * C;
    xx        = _xx;
    xy        = _xy;
    yx        = _yx;
    yy        = _yy;
    return *this;
  }
};

using TransformMatrix2 = Matrix2;
using TransformMatrix3 = Matrix3;
using TransformMatrix  = Matrix2;

} // namespace pl2d
