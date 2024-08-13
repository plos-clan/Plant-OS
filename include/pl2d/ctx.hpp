#pragma once
#include "pixel.hpp"
#include "points.hpp"
#include "texture.hpp"

namespace pl2d {

__Pclass__(Context);

struct Context : RefCount {
  TextureB *tex     = null;
  bool      own_tex = false; // tex 是否为该结构体所有
  Rect      rect;            // 允许绘图的区域

  explicit Context(TextureB *tex); // 通过现有纹理创建绘图上下文
  Context(u32 width, u32 height);  // 创建纹理并创建绘图上下文

  auto clear() {}

  auto ready() const -> bool {
    return tex != null && tex->ready();
  }
};

} // namespace pl2d
