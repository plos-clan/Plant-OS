#pragma once
#include "../point.hpp"
#include <define.h>

namespace pl2d {

template <typename T>
struct BaseRow {
  T x1 = 0, x2 = 0, y = 0;

  BaseRow(T x1, T x2, T y) : y(y) {
    cpp::exch_if(x1 > x2, x1, x2);
    this->x1 = x1, this->x2 = x2;
  }

  auto size() -> size_t {
    return (x2 - x1 + 1);
  }

  auto left() -> BasePoint2<T> {
    return {x1, y};
  }
  auto right() -> BasePoint2<T> {
    return {x2, y};
  }
};

using RowI = BaseRow<i32>;
using RowF = BaseRow<f32>;
using RowD = BaseRow<f64>;
using Row  = RowI;

class ItRowI : RowI {
public:
  using RowI::RowI;

  class Iterator {
  private:
    i32 x1, x2, y;

  public:
    Iterator(i32 x1, i32 x2, i32 y) : x1(x1), x2(x2), y(y) {}

    auto operator*() -> Point2I {
      return {x1, y};
    }

    auto operator++() -> Iterator & {
      x1++;
      return *this;
    }

    auto operator==(const Iterator &it) const -> bool {
      return x1 == it.x1;
    }

    auto operator!=(const Iterator &it) const -> bool {
      return x1 != it.x1;
    }
  };

  auto begin() const -> Iterator {
    return {x1, x2, y};
  }

  auto end() const -> Iterator {
    return {x2 + 1, x2, y};
  }
};

} // namespace pl2d
