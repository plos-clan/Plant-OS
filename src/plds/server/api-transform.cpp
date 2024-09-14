#include <pl2d.hpp>
#include <pl2d/fb.h>
#include <plds.hpp>
#include <plds/server/api.h>

dlexport auto plds_init(void *buffer, u32 width, u32 height, pl2d_PixFmt fmt) -> int {
  return plds::init(buffer, width, height, (pl2d::PixFmt)fmt);
}
dlexport void plds_flush() {
  plds::flush();
}
dlexport void plds_deinit() {
  plds::deinit();
}
dlexport auto plds_on_screen_resize(void *buffer, u32 width, u32 height, pl2d_PixFmt fmt) -> int {
  return plds::on::screen_resize(buffer, width, height, (pl2d::PixFmt)fmt);
}
dlexport void plds_on_mouse_move(i32 x, i32 y) {
  plds::on::mouse_move(x, y);
}
dlexport void plds_on_button_down(i32 button, i32 x, i32 y) {
  plds::on::button_down(button, x, y);
}
dlexport void plds_on_button_up(i32 button, i32 x, i32 y) {
  plds::on::button_up(button, x, y);
}
dlexport void plds_on_key_down(i32 key) {
  plds::on::key_down(key);
}
dlexport void plds_on_key_up(i32 key) {
  plds::on::key_up(key);
}
dlexport void plds_on_scroll(i32 nrows) {
  plds::on::scroll(nrows);
}
