#pragma once
#include "../point.hpp"
#include "col.hpp"
#include "row.hpp"
#include <define.h>

namespace pl2d {

template <typename T>
struct BaseCol {
  T x = 0, y1 = 0, y2 = 0;

  BaseCol(T x, T y1, T y2) : x(x) {
    cpp::exch_if(y1 > y2, y1, y2);
    this->y1 = y1, this->y2 = y2;
  }

  auto size() -> size_t {
    return (y2 - y1 + 1);
  }

  auto top() -> BasePoint2<T> {
    return {x, y1};
  }
  auto bottom() -> BasePoint2<T> {
    return {x, y2};
  }
};

using ColI = BaseCol<i32>;
using ColF = BaseCol<f32>;
using ColD = BaseCol<f64>;
using Col  = ColI;

class ItColI : ColI {
public:
  using ColI::ColI;

  class Iterator {
  private:
    i32 x, y1, y2;

  public:
    Iterator(i32 x, i32 y1, i32 y2) : x(x), y1(y1), y2(y2) {}

    auto operator*() -> Point2I {
      return {x, y1};
    }

    auto operator++() -> Iterator & {
      y1++;
      return *this;
    }

    auto operator==(const Iterator &it) const -> bool {
      return y1 == it.y1;
    }

    auto operator!=(const Iterator &it) const -> bool {
      return y1 != it.y1;
    }
  };

  auto begin() const -> Iterator {
    return {x, y1, y2};
  }

  auto end() const -> Iterator {
    return {x, y2 + 1, y2};
  }
};

} // namespace pl2d
