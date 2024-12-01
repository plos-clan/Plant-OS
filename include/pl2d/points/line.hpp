#pragma once
#include "../point.hpp"
#include "rect.hpp"
#include <define.h>

namespace pl2d {

template <typename T>
struct BaseLine {
  T x1 = 0, y1 = 0;
  T x2 = 0, y2 = 0;

  BaseLine()                                         = default;
  BaseLine(const BaseLine &)                         = default;
  BaseLine(BaseLine &&) noexcept                     = default;
  auto operator=(const BaseLine &) -> BaseLine     & = default;
  auto operator=(BaseLine &&) noexcept -> BaseLine & = default;

  auto translate(T x, T y) -> BaseLine & {
    x1 += x;
    x2 += x;
    y1 += y;
    y2 += y;
    return *this;
  }

  auto rect() -> BaseRect<T> {
    return {x1, y1, x2, y2};
  }

  // 将线段剪切，保留矩形内的区域
  // 如果线段完全在 rect 外部，返回 false，否则返回 true
  auto clamp(const BaseRect<T> &rect) -> bool;
  // 将线段剪切，保留矩形内的区域
  // 剪切后的线段保存在 l 中
  // 如果线段完全在 rect 外部，返回 false，否则返回 true
  auto clamp(const BaseRect<T> &rect, BaseLine &l) const -> bool;
};

using LineI = BaseLine<i32>;
using LineF = BaseLine<f32>;
using LineD = BaseLine<f64>;
using Line  = LineI;

class ItLineI : LineI {
  using LineI::LineI;

public:
  class Iterator {
  private:
    bool ended = false;
    i32  x1, y1, x2, y2;
    i32  dx, dy, sx, sy, err;

  public:
    Iterator() : ended(true) {}
    Iterator(i32 x1, i32 y1, i32 x2, i32 y2) : x1(x1), y1(y1), x2(x2), y2(y2) {
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
    return {x1, y1, x2, y2};
  }

  auto end() const -> Iterator {
    return {};
  }
};

using ItLine = ItLineI;

} // namespace pl2d
