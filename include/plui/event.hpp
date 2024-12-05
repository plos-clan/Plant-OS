#pragma once
#include <pl2d.hpp>

namespace plui {
struct Element;
}

namespace plui::cb {

// 对于所有回调：处理成功返回true，否则返回false
// 返回 true 则不继续传递，返回 false 则进一步传递给父元素

// 所有回调包含 element 参数，用来确定收到消息的元素
// 可以配置 element 为初始收到消息的元素 或 当前收到消息的元素

/**
 *\brief 元素大小改变时的回调函数
 *
 *\param[in] element 元素的唯一标识符
 *\param[in] width 改变后的宽度
 *\param[in] height 改变后的高度
 *\return bool, 如果处理回调成功则返回true，否则返回false
 */
using resize_t = bool (*)(Element &element, u32 width, u32 height);

/**
 *\brief 任何东西移动时的回调函数
 *
 *\param[in] element 元素的唯一标识符
 *\param[in] x 改变后的 x 坐标
 *\param[in] y 改变后的 y 坐标
 *\return bool, 如果处理回调成功则返回true，否则返回false
 */
using move_t = bool (*)(Element &, i32, i32);

using button_t = bool (*)(Element &, i32, i32, i32);

using key_t = bool (*)(Element &, i32);

// 向下滚动的行数
using scroll_t = bool (*)(Element &, i32);

// 某个元素成为该元素的子元素
// 某个子元素从该元素中分离
using child_t = bool (*)(Element &, Element &);

// 元素需要被重绘
using draw_t = bool (*)(Element &, pl2d::Texture &);

using event_t = bool (*)(Element &);

struct Callbacks {
  resize_t style;         //
                          //
  move_t   before_draw;   // 元素重绘之前
  draw_t   draw;          // 元素重绘
  event_t  need_draw;     // 元素需要重绘
  resize_t resize;        // 元素大小更改
  move_t   moveto;        // 元素移动
  move_t   move;          // 元素移动
                          //
  move_t   mouse_move;    // 鼠标移动
  event_t  mouse_enter;   // 鼠标进入
  event_t  mouse_leave;   // 鼠标离开
                          //
  button_t click;         // 鼠标按钮点击（按下并释放）
  button_t doubleclick;   // 鼠标按钮双击（在指定时间内按下并释放两次）
  button_t button_down;   // 鼠标按钮按下
  button_t button_up;     // 鼠标按钮释放
                          //
  key_t    key_down;      // 键盘按键按下
  key_t    key_up;        // 键盘按键释放
  key_t    key;           // 键盘按键点击（按下并释放）
                          //
  scroll_t scroll;        // 滚动
                          //
  child_t  child;         // 子元素添加
  child_t  child_destroy; // 子元素将被销毁
                          //
  event_t  destroy;       // 元素将被销毁
                          //
  event_t  focus;         // 元素获得焦点
  event_t  focus_lost;    // 元素失去焦点
                          //
  event_t  show;          // 元素被显示
  event_t  hide;          // 元素被隐藏
};

}; // namespace plui::cb

namespace plui {
using Callbacks = cb::Callbacks;
}
