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

  auto operator=(const Position &) -> Position &     = default;
  auto operator=(Position &&) noexcept -> Position & = default;

  INLINE_CONST auto move(i32 dx, i32 dy) noexcept -> Position & {
    x += dx, y += dy;
    return *this;
  }

  INLINE_CONST auto moveto(i32 x, i32 y) noexcept -> Position & {
    this->x = x, this->y = y;
    return *this;
  }

  INLINE_CONST auto topleft() const noexcept -> pl2d::Point2I {
    return {x, y};
  }
  INLINE_CONST auto topright() const noexcept -> pl2d::Point2I {
    return {x + w - 1, y};
  }
  INLINE_CONST auto bottomleft() const noexcept -> pl2d::Point2I {
    return {x, y + h - 1};
  }
  INLINE_CONST auto bottomright() const noexcept -> pl2d::Point2I {
    return {x + w - 1, y + h - 1};
  }

  INLINE_CONST auto contains(i32 _x, i32 _y) const noexcept -> bool {
    if (_x < x || _x >= x + w) return false;
    if (_y < y || _y >= y + h) return false;
    return true;
  }
  INLINE_CONST auto contains(const pl2d::Point2I &p) const noexcept -> bool {
    return contains(p.x, p.y);
  }

  INLINE_CONST auto rect() const -> pl2d::Rect {
    return {x, y, x + w, y + h};
  }
};

}; // namespace plui
