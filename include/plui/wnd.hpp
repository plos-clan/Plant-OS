#pragma once
#include "element.hpp"

namespace plui {

struct pdui_window : Element {
  char   *title;
  bool    moving; // 是否正在移动窗口
  int32_t oldx;
  int32_t oldy;
  int8_t  order; // 窗口排列的顺序，置顶、置底
};

} // namespace plui
