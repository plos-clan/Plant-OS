#pragma once
#include <pl2d.hpp>

namespace plui {

struct Position {
  i32 x = 0, y = 0; // 左上角坐标
  i32 w = 0, h = 0; // 右下角坐标

  Position() = default;
  Position(i32 x, i32 y, i32 w, i32 h) : x(x), y(y), w(w), h(h) {}
  Position(const Position &)     = default;
  Position(Position &&) noexcept = default;

  auto operator=(const Position &) -> Position     & = default;
  auto operator=(Position &&) noexcept -> Position & = default;

  auto move(i32 dx, i32 dy) -> Position & {
    x += dx, y += dy;
    return *this;
  }

  auto moveto(i32 x, i32 y) -> Position & {
    this->x = x, this->y = y;
    return *this;
  }

  auto topleft() const -> pl2d::Point2I {
    return {x, y};
  }
  auto topright() const -> pl2d::Point2I {
    return {x + w - 1, y};
  }
  auto bottomleft() const -> pl2d::Point2I {
    return {x, y + h - 1};
  }
  auto bottomright() const -> pl2d::Point2I {
    return {x + w - 1, y + h - 1};
  }

  auto contains(i32 _x, i32 _y) const -> bool {
    if (_x < x || _x >= x + w) return false;
    if (_y < y || _y >= y + h) return false;
    return true;
  }
  auto contains(const pl2d::Point2I &p) const -> bool {
    return contains(p.x, p.y);
  }
};

}; // namespace plui
