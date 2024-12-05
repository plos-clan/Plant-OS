#pragma once
#include <plds/base.hpp>

extern "C" {
void program_exit();
void screen_flush();
}

namespace plds {

auto init(void *buffer, u32 width, u32 height, pl2d::PixFmt fmt = pl2d::texture_pixfmt) -> int;
void flush();
void deinit();

namespace event {

enum {
  pointer_type_unknown = 0, // 未知设备
  pointer_type_mouse   = 1, // 鼠标
  pointer_type_pen     = 2, // 笔
  pointer_type_touch   = 3, // 触摸
};

// 可参考：https://developer.mozilla.org/en-US/docs/Web/API/PointerEvent
struct Pointer {
  i32          deviceid; // 设备 ID
  u32          type;     // 指针设备类型
  i32          x, y;     // 鼠标坐标
  u32          button;   // 0 到 31 位分别代表按钮 1 到 32
  i32          nbuttons; // 鼠标拥有的按键数量
  f32          pressure; // 压力，若设备不支持则按下为 0.5，松开为 0
  pl2d::Point2 tilt;     // https://developer.mozilla.org/en-US/docs/Web/API/PointerEvent/tiltX
  f32          twist;    // https://developer.mozilla.org/en-US/docs/Web/API/PointerEvent/twist
};

struct Key {
  u32 id; // 键盘按键 ID
};

} // namespace event

struct Event {
  u32            type;    // 事件类型
  event::Key     key;     // 键盘按键
  event::Pointer pointer; // 鼠标事件
  bool           left;    // 是否按下鼠标左键
  bool           middle;  // 是否按下鼠标中键
  bool           right;   // 是否按下鼠标右键
  bool           shift;   // 是否按下 Shift 键
  bool           ctrl;    // 是否按下 Ctrl 键
  bool           alt;     // 是否按下 Alt 键
  bool           super;   // 是否按下 Super 键
};

} // namespace plds

namespace plds::event {

enum {
  LButton    = 0,
  MButton    = 1,
  RButton    = 2,
  ScrollUp   = 3,
  ScrollDown = 4,

  BackSpace   = 0xff08,
  Tab         = 0xff09,
  Linefeed    = 0xff0a,
  Clear       = 0xff0b,
  Return      = 0xff0d,
  Pause       = 0xff13,
  Scroll_Lock = 0xff14,
  Sys_Req     = 0xff15,
  Escape      = 0xff1b,
  Delete      = 0xffff,

  Home      = 0xff50,
  Left      = 0xff51,
  Up        = 0xff52,
  Right     = 0xff53,
  Down      = 0xff54,
  Prior     = 0xff55,
  Page_Up   = 0xff55,
  Next      = 0xff56,
  Page_Down = 0xff56,
  End       = 0xff57,
  Begin     = 0xff58,

  Select        = 0xff60,
  Print         = 0xff61,
  Execute       = 0xff62,
  Insert        = 0xff63,
  Undo          = 0xff65,
  Redo          = 0xff66,
  Menu          = 0xff67,
  Find          = 0xff68,
  Cancel        = 0xff69,
  Help          = 0xff6a,
  Break         = 0xff6b,
  Mode_switch   = 0xff7e,
  script_switch = 0xff7e,
  Num_Lock      = 0xff7f,

  P_Space     = 0xff80,
  P_Tab       = 0xff89,
  P_Enter     = 0xff8d,
  P_F1        = 0xff91,
  P_F2        = 0xff92,
  P_F3        = 0xff93,
  P_F4        = 0xff94,
  P_Home      = 0xff95,
  P_Left      = 0xff96,
  P_Up        = 0xff97,
  P_Right     = 0xff98,
  P_Down      = 0xff99,
  P_Prior     = 0xff9a,
  P_Page_Up   = 0xff9a,
  P_Next      = 0xff9b,
  P_Page_Down = 0xff9b,
  P_End       = 0xff9c,
  P_Begin     = 0xff9d,
  P_Insert    = 0xff9e,
  P_Delete    = 0xff9f,
  P_Equal     = 0xffbd,
  P_Multiply  = 0xffaa,
  P_Add       = 0xffab,
  P_Separator = 0xffac,
  P_Subtract  = 0xffad,
  P_Decimal   = 0xffae,
  P_Divide    = 0xffaf,

  P_0 = 0xffb0,
  P_1 = 0xffb1,
  P_2 = 0xffb2,
  P_3 = 0xffb3,
  P_4 = 0xffb4,
  P_5 = 0xffb5,
  P_6 = 0xffb6,
  P_7 = 0xffb7,
  P_8 = 0xffb8,
  P_9 = 0xffb9,

  F1  = 0xffbe,
  F2  = 0xffbf,
  F3  = 0xffc0,
  F4  = 0xffc1,
  F5  = 0xffc2,
  F6  = 0xffc3,
  F7  = 0xffc4,
  F8  = 0xffc5,
  F9  = 0xffc6,
  F10 = 0xffc7,
  F11 = 0xffc8,
  F12 = 0xffc9,
  F13 = 0xffca,
  F14 = 0xffcb,
  F15 = 0xffcc,
  F16 = 0xffcd,
  F17 = 0xffce,
  F18 = 0xffcf,
  F19 = 0xffd0,
  F20 = 0xffd1,
  F21 = 0xffd2,
  F22 = 0xffd3,
  F23 = 0xffd4,
  F24 = 0xffd5,

  LShift    = 0xffe1,
  RShift    = 0xffe2,
  LControl  = 0xffe3,
  RControl  = 0xffe4,
  CapsLock  = 0xffe5,
  ShiftLock = 0xffe6,

  LMeta  = 0xffe7,
  RMeta  = 0xffe8,
  LAlt   = 0xffe9,
  RAlt   = 0xffea,
  LSuper = 0xffeb,
  RSuper = 0xffec,
  LHyper = 0xffed,
  RHyper = 0xffee,

  space        = 0x0020,
  exclam       = 0x0021,
  quotedbl     = 0x0022,
  numbersign   = 0x0023,
  dollar       = 0x0024,
  percent      = 0x0025,
  ampersand    = 0x0026,
  apostrophe   = 0x0027,
  quoteright   = 0x0027,
  parenleft    = 0x0028,
  parenright   = 0x0029,
  asterisk     = 0x002a,
  plus         = 0x002b,
  comma        = 0x002c,
  minus        = 0x002d,
  period       = 0x002e,
  slash        = 0x002f,
  K0           = 0x0030,
  K1           = 0x0031,
  K2           = 0x0032,
  K3           = 0x0033,
  K4           = 0x0034,
  K5           = 0x0035,
  K6           = 0x0036,
  K7           = 0x0037,
  K8           = 0x0038,
  K9           = 0x0039,
  colon        = 0x003a,
  semicolon    = 0x003b,
  less         = 0x003c,
  equal        = 0x003d,
  greater      = 0x003e,
  question     = 0x003f,
  at           = 0x0040,
  A            = 0x0041,
  B            = 0x0042,
  C            = 0x0043,
  D            = 0x0044,
  E            = 0x0045,
  F            = 0x0046,
  G            = 0x0047,
  H            = 0x0048,
  I            = 0x0049,
  J            = 0x004a,
  K            = 0x004b,
  L            = 0x004c,
  M            = 0x004d,
  N            = 0x004e,
  O            = 0x004f,
  P            = 0x0050,
  Q            = 0x0051,
  R            = 0x0052,
  S            = 0x0053,
  T            = 0x0054,
  U            = 0x0055,
  V            = 0x0056,
  W            = 0x0057,
  X            = 0x0058,
  Y            = 0x0059,
  Z            = 0x005a,
  bracketleft  = 0x005b,
  backslash    = 0x005c,
  bracketright = 0x005d,
  asciicircum  = 0x005e,
  underscore   = 0x005f,
  grave        = 0x0060,
  quoteleft    = 0x0060,
  a            = 0x0061,
  b            = 0x0062,
  c            = 0x0063,
  d            = 0x0064,
  e            = 0x0065,
  f            = 0x0066,
  g            = 0x0067,
  h            = 0x0068,
  i            = 0x0069,
  j            = 0x006a,
  k            = 0x006b,
  l            = 0x006c,
  m            = 0x006d,
  n            = 0x006e,
  o            = 0x006f,
  p            = 0x0070,
  q            = 0x0071,
  r            = 0x0072,
  s            = 0x0073,
  t            = 0x0074,
  u            = 0x0075,
  v            = 0x0076,
  w            = 0x0077,
  x            = 0x0078,
  y            = 0x0079,
  z            = 0x007a,
  braceleft    = 0x007b,
  bar          = 0x007c,
  braceright   = 0x007d,
  asciitilde   = 0x007e,
};

} // namespace plds::event

namespace plds::on {

// 屏幕大小重设
auto screen_resize(void *buffer, u32 width, u32 height, pl2d::PixFmt fmt = pl2d::texture_pixfmt)
    -> int;
// 鼠标移动
void mouse_move(i32 x, i32 y);
// 鼠标按键按下
void button_down(i32 button, i32 x, i32 y);
// 鼠标按键释放
void button_up(i32 button, i32 x, i32 y);
// 键盘按键按下
void key_down(i32 key);
// 键盘按键释放
void key_up(i32 key);
// 鼠标滚轮，nrows 表示内容可见区域向下 N 行
void scroll(i32 nrows);

} // namespace plds::on
