#pragma once
#include "event.hpp"
#include "pos.hpp"
#include <data-structure.hpp>
#include <pl2d.hpp>

namespace plui {

struct Style {

  u32 padding;

  u32         border_width; // 边框宽度
  pl2d::Pixel border_color; // 边框颜色

  pl2d::Pixel bg_color = 0xffffffff; // 背景色
  pl2d::Pixel fg_color = 0x000000ff; // 前景色

  pl2d::Texture *bg_img; // 背景图像

  bool moveable;   // 是否可以移动
  bool resizeable; // 是否可以调整大小

  char *text;

  // font;

  bool fit_content;

  Style() = default;
};

using ChildList = cpp::List<Element *>;

struct Element : Style {
  Position       pos;                 // 位置
  pl2d::Rect     internal;            // 元素内部的区域
  Element       *parent;              // 父元素，表示当前元素的上一级元素
  bool           needdraw;            // 是否需要刷新，表示是否需要重新绘制元素
  pl2d::Texture *draw_tex = null;     // 可绘制对象，用于绘制元素的图形
  pl2d::Context *draw_ctx = null;     // 可绘制对象，用于绘制元素的图形
  bool           receive_child_event; // 是否接收子对象的事件
  Callbacks      cb;                  // 事件处理器
  bool           mouse_hover;         //
  bool           visible;             // 是否可见
  bool           lock;                // 是否被锁定，锁定状态下无法修改属性
  bool           focus;               // 是否获得焦点
  bool           nochild;             // 是否不允许拥有子元素
  bool           always_on_top;       // 元素总在顶部
  bool           always_on_bottom;    // 元素总在底部
  ChildList      child;               // 子元素列表

  Element() = default;
};

} // namespace plui
